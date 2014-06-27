#include <boost/filesystem.hpp>
#include <boost/python.hpp>
#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include "api.h"
#include "gil.h"

namespace py = boost::python;
std::vector<std::thread> threads;
std::timed_mutex kill_thread_finish_signal;

// volatile needed to prevent fallacious optimizations.
// This is a hack anyway.
volatile long thread_id;
PyInterpreterState *main_interpreter_state;

///
/// Wrapper function for std::timed_mutex::try_lock_for which works around bugs in
/// the implementations in certain compilers.
///
/// Waiting can be more, but not less than, the required period, assuming that
/// the user's clock is working as expected.
///
/// @param        lock The mutex to wait on
/// @param time_period The minimal length to wait on.
///
bool try_lock_for_busywait(std::timed_mutex &lock, std::chrono::nanoseconds time_period) {
    auto end = std::chrono::system_clock::now() + time_period;

    while (true) {
        // If lock is taken, return true. Otherwise, try to delay
        if (lock.try_lock_for(end - std::chrono::system_clock::now())) {
            return true;
        }

        // If delay has passed, hasn't been locked so return false
        if (end <= std::chrono::system_clock::now()) {
            return false;
        }

        // Make sure not a busy wait on broken compilers
        std::this_thread::sleep_for(time_period / 100);
    }
}

///
/// Kills threads. Currently only supports a single thread.
/// Uses globals kill_thread_finish_signal and thread_id.
///
void thread_killer() {
    py::dict tempoary_scope;
    py::api::object killer_func;

    {
        GIL lock_gil;

        py::exec(
            "def killer_func(frame, event, arg):\n"
            "    if event == 'line':\n"
            "        1/0\n"
            "    else:\n"
            "        return killer_func\n",
            tempoary_scope
        );
        killer_func = tempoary_scope["killer_func"];
    }

    while (!thread_id) {}
    std::cout << thread_id << std::endl;

    while (true) {
        long previous_call_number = Player::call_number;

        // Nonbloking sleep; allows safe quit
        if (try_lock_for_busywait(kill_thread_finish_signal, std::chrono::milliseconds(100))) {
            break;
        }

        if (Player::call_number == previous_call_number) {
            GIL lock_gil;
            std::cout << "Attempting to kill thread id " << thread_id << "." << std::endl;
            PyThreadState_SetAsyncExc(thread_id, PyExc_SystemError);
        }
    }

    std::cout << "Finished KILLER thread" << std::endl;
}


///
/// A thread function representing a player character.
///
/// @param code        C++ string representing the code to be exeuted.
/// @param player      Reference to the Player object that moderates execution.
/// @param working_dir Path, as a string, to inject into Python's sys.path, to allow relative imports.
///                    This should be the current path, to allow importing our shared object files.
///
void _spawn_thread(std::string code, py::api::object player, std::string working_dir) {
    auto threadstate = PyThreadState_New(main_interpreter_state);

    {
        PyEval_RestoreThread(threadstate);

        try {
            // Append the working directory to Python's sys.path
            py::import("sys").attr("path").attr("append")(py::str(working_dir));

            // The main module's namespace is needed to evaluate code
            // and the main module object is needed to have
            // juicy, juicy introspection and monkey-patching.
            auto main_module = py::import("__main__");
            auto main_namespace = main_module.attr("__dict__");

            // Inject a player; will be instantiated in C++
            main_module.attr("player") = player;

            // Pass the Vec2D class. Should be neater, but it's not obvious how.
            // Currently this gets the class object from an instance object.
            main_module.attr("Vec2D") = py::object(Vec2D(0, 0)).attr("__class__");

            // Totally a hack. Remove ASA(R)P.
            auto py_thread_id = py::import("threading").attr("current_thread")().attr("ident");
            thread_id = py::extract<long>(py_thread_id);

            py::exec(code.c_str(), main_namespace);
        }
        catch (py::error_already_set) {
            
            PyErr_Print();
        }

        PyThreadState_Clear(threadstate);
        /*threadstate =*/PyEval_SaveThread();
    }

    std::cout << "Finishing RUNNER thread" << std::endl;
    PyThreadState_Delete(threadstate);
    std::cout << "Finished RUNNER thread" << std::endl;
}

///
/// Creates a thread representing a player character.
///
/// @param code        C++ string representing the code to be exeuted.
/// @param player      Reference to the Player object that moderates execution.
/// @param working_dir Path, as a string, to inject into Python's sys.path, to allow relative imports.
///                    This should be the current path, to allow importing our shared object files.
///
void spawn_thread(std::string code, py::api::object player, std::string working_dir) {
    threads.push_back(std::thread(_spawn_thread, code, player, working_dir));
}

///
/// Initialize Python interpreter, spawn threads and do fun stuff.
///
int main(int, char **) {
    Py_Initialize();
    PyEval_InitThreads();

    // Produe a locked mutex; unlock to allow thread_killer to die
    kill_thread_finish_signal.lock();
    std::thread kill_thread(thread_killer);

    auto main_thread_state = PyThreadState_Get();
    main_interpreter_state = main_thread_state->interp;

    Player player = Player(Vec2D(0, 0), "");

    std::string working_dir;

    // All Python errors should result in a Python traceback    
    try {
        auto sys_module = py::import("sys");
        working_dir = boost::filesystem::absolute("./").normalize().string();

        sys_module.attr("path").attr("append")(py::str(working_dir));
        py::import("wrapper_functions");

        player = Player(Vec2D(0, 0), "John");
    } catch (py::error_already_set) {
        std::cout << "Basic error! Complain!" << std::endl;
        PyErr_Print();
    }

    try {
    for (int i = 0; i < 1; ++i) {
        spawn_thread(
                "print('--- Inside " + std::to_string(i) + " ---')\n"
                "player.monologue()\n"
                "try:\n"
                "    for i in range(4):\n"
                "        player.give_script(globals())\n"
                "        player.run_script()\n"
                "        import time\n"
                "        for _ in range(10**i):\n"
                "           time.sleep(0.01)\n"
                "except BaseException as e:\n"
                "    print('Halted with {}.'.format(type(e)))\n"
                "    raise\n",
                py::object(boost::ref(player)),
                working_dir
        );
    }
    } catch (py::error_already_set &) {
        PyErr_Print();
    }

    PyEval_ReleaseLock();

    for (auto &thread : threads) {
        thread.join();
    }

    kill_thread_finish_signal.unlock();
    kill_thread.join();

    PyEval_RestoreThread(main_thread_state);
}

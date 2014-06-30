#include <boost/filesystem.hpp>
#include <boost/python.hpp>
#include <chrono>
#include <future>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include "api.h"
#include "debug.h"
#include "locks.h"
#include "make_unique.h"
#include "playerthread.h"

namespace py = boost::python;

std::timed_mutex kill_thread_finish_signal;
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
void thread_killer(std::vector<PlayerThread> &playerthreads) {
    while (true) {
        // Nonbloking sleep; allows safe quit
        if (try_lock_for_busywait(kill_thread_finish_signal, std::chrono::milliseconds(100))) {
            break;
        }

        // Go through the available playerthread objects and kill those that
        // haven't had an API call.
        for (auto &playerthread : playerthreads) {
            if (!playerthread.is_dirty()) {
                GIL lock_gil;

                print_debug << "Attempting to kill thread id " << playerthread.thread_id << "." << std::endl;

                PyThreadState_SetAsyncExc(playerthread.thread_id, PyExc_SystemError);
            }
            playerthread.set_clean();
        }
    }

    print_debug << "Finished KILLER thread" << std::endl;
}


///
/// A thread function representing a player character.
///
/// @param code        C++ string representing the code to be exeuted.
/// @param player      Reference to the Player object that moderates execution.
/// @param working_dir Path, as a string, to inject into Python's sys.path, to allow relative imports.
///                    This should be the current path, to allow importing our shared object files.
///
void run_player(std::string code,
                py::api::object player,
                std::promise<int64_t> thread_id_promise,
                std::string working_dir) {

    print_debug << "run_player: Starting" << std::endl;

    ThreadState threadstate(main_interpreter_state);
    ThreadGIL lock_thread(threadstate);

    print_debug << "run_player: Stolen GIL" << std::endl;

    try {
        // Append the working directory to Python's sys.path
        py::import("sys").attr("path").attr("append")(py::str(working_dir));

        // The main module's namespace is needed to evaluate code
        // and the main module object is needed to have
        // juicy, juicy introspection and monkey-patching.
        auto main_module = py::import("__main__");
        auto main_namespace = main_module.attr("__dict__").attr("copy")();

        // Inject a player; will be instantiated in C++
        main_namespace["player"] = player;

        // Pass the Vec2D class. Should be neater, but it's not obvious how.
        // Currently this gets the class object from an instance object.
        main_namespace["Vec2D"] = py::object(Vec2D(0, 0)).attr("__class__");

        print_debug << "run_player: Basic setup" << std::endl;
        // Totally a hack. Remove ASA(R)P.
        auto py_thread_id = py::import("threading").attr("current_thread")().attr("ident");
        thread_id_promise.set_value(py::extract<int64_t>(py_thread_id));

        print_debug << "run_player: Gave promise" << std::endl;

        py::exec(code.c_str(), main_namespace);

        print_debug << "run_player: Executed code" << std::endl;
    }
    catch (py::error_already_set &) {
        print_debug << "run_player: Python-side error" << std::endl;
        PyErr_Print();
    }

    print_debug << "run_player: Finished" << std::endl;
}

///
/// Creates a thread representing a player character.
///
/// @param code        C++ string representing the code to be exeuted.
/// @param player      Reference to the Player object that moderates execution.
/// @param working_dir Path, as a string, to inject into Python's sys.path, to allow relative imports.
///                    This should be the current path, to allow importing our shared object files.
///
void spawn_thread(std::string code,
                  Player &player,
                  std::vector<PlayerThread> &playerthreads,
                  std::string working_dir) {

    print_debug << "spawn_thread: Starting" << std::endl;

    std::promise<int64_t> thread_id_promise;
    auto thread_id_future = thread_id_promise.get_future();

    // This seems to be the easy compromise.
    // http://stackoverflow.com/questions/24477791
    py::api::object player_object = [&player] () {
        GIL lock_gil;
        return py::api::object(boost::ref(player));
    } ();

    auto thread = std::make_unique<std::thread>(
        run_player,
        code,
        player_object,
        std::move(thread_id_promise),
        working_dir
    );

    print_debug << "spawn_thread: Made thread" << std::endl;

    thread_id_future.wait();

    print_debug << "spawn_thread: Got thread ID" << std::endl;

    playerthreads.push_back(PlayerThread(
        player,
        std::move(thread),
        thread_id_future.get()
    ));

    print_debug << "spawn_thread: Created PlayerThread" << std::endl;
}

///
/// Initialize Python interpreter, spawn threads and do fun stuff.
///
int main(int, char **) {
    Py_Initialize();
    PyEval_InitThreads();

    print_debug << "main: Initialized Python" << std::endl;

    // Produe a locked mutex; unlock to allow thread_killer to die
    std::vector<PlayerThread> playerthreads;
    kill_thread_finish_signal.lock();
    std::thread kill_thread(thread_killer, std::ref(playerthreads));

    print_debug << "main: Spawned Kill thread" << std::endl;

    auto main_thread_state = PyThreadState_Get();
    main_interpreter_state = main_thread_state->interp;

    Player playerJ = Player(Vec2D(0, 0), "John");
    Player playerA = Player(Vec2D(0, 0), "Adam");
    std::string working_dir;

    // All Python errors should result in a Python traceback    
    try {
        auto sys_module = py::import("sys");
        working_dir = boost::filesystem::absolute("./").normalize().string();

        sys_module.attr("path").attr("append")(py::str(working_dir));
        py::import("wrapper_functions");
    }
    catch (py::error_already_set) {
        print_debug << "main: Unexpected error setting path" << std::endl;
        PyErr_Print();
        return 1;
    }

    print_debug << "main: Set path" << std::endl;

    PyEval_ReleaseLock();
    print_debug << "main: Released GIL " << std::endl;

    spawn_thread(
            "print('--- Inside " + std::to_string(0) + " ---')\n"
            "player.monologue()\n"
            "try:\n"
            "    player.give_script(globals())\n"
            "    for i in range(4):\n"
            "        player.run_script()\n"
            "        import time\n"
            "        for _ in range(int(10**(i-1))):\n"
            "           time.sleep(0.001)\n"
            "except BaseException as e:\n"
            "    print('Halted with {}.'.format(type(e)))\n"
            "    raise\n",
            playerJ,
            playerthreads,
            working_dir
    );
    print_debug << "main: Spawned PlayerThread " << 0 << std::endl;

    spawn_thread(
            "print('--- Inside " + std::to_string(0) + " ---')\n"
            "player.monologue()\n"
            "try:\n"
            "    player.give_script(globals())\n"
            "    for i in range(4):\n"
            "        player.run_script()\n"
            "        import time\n"
            "        for _ in range(int(10**(i-1))):\n"
            "           time.sleep(0.001)\n"
            "except BaseException as e:\n"
            "    print('Halted with {}.'.format(type(e)))\n"
            "    raise\n",
            playerA,
            playerthreads,
            working_dir
    );
    print_debug << "main: Spawned PlayerThread " << 0 << std::endl;

    for (auto &playerthread : playerthreads) {
        playerthread.thread->join();
        print_debug << "main: Joined a PlayerThread" << std::endl;
    }

    print_debug << "main: Joined all PlayerThreads" << std::endl;

    kill_thread_finish_signal.unlock();
    kill_thread.join();

    print_debug << "main: Joined kill_thread" << std::endl;

    PyEval_RestoreThread(main_thread_state);

    return 0;
}

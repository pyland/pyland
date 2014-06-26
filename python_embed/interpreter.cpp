#include <boost/filesystem.hpp>
#include <boost/python.hpp>
#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include "api.h"

namespace py = boost::python;
std::vector<std::thread> threads;
std::timed_mutex kill_thread_finish_signal;

long thread_id;
std::string working_dir;
PyInterpreterState *main_interpreter_state;

void thread_killer() {
    py::dict tempoary_scope;
    py::api::object killer_func;

    {
        auto gstate = PyGILState_Ensure();

        py::exec(
            "def killer_func(frame, event, arg):\n"
            "    if event == 'line':\n"
            "        1/0\n"
            "    else:\n"
            "        return killer_func\n",
            tempoary_scope
        );
        killer_func = tempoary_scope["killer_func"];
    
        PyGILState_Release(gstate);
    }

    while (!thread_id) {}
    std::cout << thread_id << std::endl;

    while (true) {
        long previous_call_number = Player::call_number;

        // Nonbloking sleep; allows safe quit
        if (kill_thread_finish_signal.try_lock_for(std::chrono::milliseconds(100))) { break; }

        if (Player::call_number == previous_call_number) {
            {
                auto gstate = PyGILState_Ensure();

                std::cout << "Attempting to kill thread id " << thread_id << "." << std::endl;
                PyThreadState_SetAsyncExc(thread_id, PyExc_SystemError);;

                PyGILState_Release(gstate);
            }
        }
    }

    std::cout << "Finished KILLER thread" << std::endl;
}


// Steals GIL
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

// Needs GIL
void spawn_thread(std::string code, py::api::object player, std::string working_dir) {
    threads.push_back(std::thread(_spawn_thread, code, player, working_dir));
}

int main(int, char **) {
    Py_Initialize();
    PyEval_InitThreads();

    // Produe a locked mutex; unlock to allow thread_killer to die
    kill_thread_finish_signal.lock();
    std::thread kill_thread(thread_killer);

    auto main_thread_state = PyThreadState_Get();
    main_interpreter_state = main_thread_state->interp;

    Player player = Player(Vec2D(0, 0), "");

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


    for (int i = 0; i < 1; ++i) {
        spawn_thread(
                "print('--- Inside " + std::to_string(i) + " ---')\n"
                "player.monologue()\n"

                "def get_script(n):\n"
                "    def script(player):\n"
                "        x_direction = Vec2D(1, 0)\n"
                "        for _ in range(n):\n"
                "            player.move(x_direction)\n"
                "        player.monologue()\n"

                "    return script\n"

                "try:\n"
                "    for i in range(4):\n"
                "        player.give_script(get_script(10**i))\n"
                "        player.run_script()\n"
                "        import time\n"
                "        for _ in range(10**i):\n"
                "           time.sleep(0.01)\n"
                "except BaseException as e:\n"
                "    print('Halted with {}.'.format(type(e)))\n",
                py::object(boost::ref(player)),
                working_dir
        );
    }

    PyEval_ReleaseLock();

    for (auto &thread : threads) {
        thread.join();
    }

    kill_thread_finish_signal.unlock();
    kill_thread.join();

    PyEval_RestoreThread(main_thread_state);
}

#include <boost/filesystem.hpp>
#include <boost/python.hpp>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include "api.h"

namespace py = boost::python;

py::api::object sys_module;
py::api::object main_module;
py::api::object main_namespace;

long thread_id;

// Perform some Python actions here

void thread_killer() {
    auto gstate = PyGILState_Ensure();

    py::dict tempoary_scope;
    py::api::object killer_func;

    py::exec(
        "def killer_func(frame, event, arg):\n"
        "    if event == 'line':\n"
        "        1/0\n"
        "    else:\n"
        "        return killer_func\n"
        ,
        tempoary_scope
    );
    killer_func = tempoary_scope["killer_func"];

    PyGILState_Release(gstate);


    while (true) {
        int previous_call_number = Player::call_number;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        if (Player::call_number == previous_call_number) {
            // WARNING! NOT THREAD SAFE!
            // WARNING! NOT THREAD SAFE!
            if (Player::in_call) {
                auto gstate = PyGILState_Ensure();

                try {
                    std::cout << "Thread id " << thread_id << " and attempting to kill." << std::endl;
                    PyThreadState_SetAsyncExc(thread_id, PyExc_LookupError);;
                    //py::exec("import threading; print(threading.current_thread())", main_namespace);
                }
                catch (boost::python::error_already_set) {
                    PyErr_Print();
                }

                PyGILState_Release(gstate);
            }
        }
    }
}

int main(int argc, char **argv) {
    Py_Initialize();

    std::thread kill_thread(thread_killer);
    kill_thread.detach();

    // All Python errors should result in a Python traceback

    try {
        // Get the current directory and add (append) it to Python's sys.path
        sys_module = py::import("sys");

        auto workingDir = boost::filesystem::absolute("./").normalize();
        auto sys_paths = sys_module.attr("path");
        sys_paths.attr("append")(py::str(workingDir.string()));

        // The main module's namespace is needed to evaluate code
        // and the main module object is needed to have
        // juicy, juicy introspection and monkey-patching.
        main_module = py::import("__main__");
        main_namespace = main_module.attr("__dict__");

        // Expose the wrappers to Python code.
        // This will be wrapped better in the final thing, with
        // a Python module to abstract C-level errors. 
        auto wrapper_module = py::import("wrapper_functions");
        main_module.attr("wrapper_functions") = wrapper_module;

        // Inject a player; will be instantiated in C++
        auto my_player = Player(Vec2D(0, 0), Direction::UP, "John");
        main_module.attr("preplayer") = py::object(boost::ref(my_player));

        // Pass the Vec2D class. Should be neater, but it's not obvious how.
        // Currently this gets the class object from an instance object.
        main_module.attr("Vec2D") = py::object(Vec2D(0, 0)).attr("__class__");

        auto py_main_thread = py::import("threading").attr("current_thread")().attr("ident");
        thread_id = py::extract<long>(py_main_thread);

        // Run the wrapper Python code
        py::exec(
                "print('--- Inside ---')\n"
                "preplayer.monologue()\n"

                "def get_script(n):\n"
                "    def script(player):\n"
                "        x_direction = Vec2D(1, 0)\n"
                "        for _ in range(n):\n"
                "            player.move(x_direction)\n"
                "        player.monologue()\n"

                "    return script\n"

                "try:\n"
                "    for i in range(100):\n"
                "        preplayer.give_script(get_script(10**i))\n"
                "        preplayer.run_script()\n"
                "except BaseException as e:\n"
                "    print('Halted with {}.'.format(type(e)))\n"
            ,
            main_namespace
        );

        // Prove that this has affected the outside environment
        std::cout << "--- Outside ---" << std::endl;
        my_player.monologue();
    }
    catch (boost::python::error_already_set) {
        PyErr_Print();
    }
}

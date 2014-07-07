#include <boost/filesystem.hpp>
#include <boost/python.hpp>
#include <mutex>
#include <playerthread.h>
#include "interpreter.h"

Interpreter::Interpreter(boost::filesystem::path working_directory,
                         boost::filesystem::path function_wrappers) {

    // Leave locked forever to prove that it's only been locked once
    if (!initialized.try_lock()) {
        throw std::runtime_error("Interpreter initialized twice where only a single initialization supported");
    }

    auto main_interpreter_state = initialize_python();

    Lockable<std::vector<PlayerThread>> playerthreads;

    thread_killer = make_unique<ThreadKiller>(playerthreads);
    print_debug << "Interpreter: Spawned Kill thread" << std::endl;

    // All Python errors should result in a Python traceback    
    try {
        // TODO: decide whether to set sys.path

        // Import to allow conversion of classes, but no need to keep module reference
        import_file(function_wrappers);
    }
    catch (py::error_already_set) {
        PyErr_Print();
    }

    // Release GIL; thread_killer can start killing now
    // and PlayerThreads can be created without deadlocks
    PyEval_ReleaseLock();

    print_debug << "Interpreter: Released GIL " << std::endl;
    print_debug << "Interpreter: Interpreter created " << std::endl;
}

PyInterpreterState *Interpreter::initialize_python() {
    Py_Initialize();
    PyEval_InitThreads();

    print_debug << "Interpreter: Initialized Python" << std::endl;

    return PyThreadState_Get()->interp;
}

void Interpreter::register(Entity entity) {
    std::lock_guard<std::mutex> lock(playerthreads_editable);

    playerthreads.push_back(PlayerThread(entity));
}

Interpreter::~Interpreter() {
    thread_killer->finish();
    print_debug << "Interpreter: Finished kill thread" << std::endl;

    // Not acutally needed; thread_killer is dead
    // However, this keeps the guarantees (locked while edited) safe,
    // so is good practice
    std::lock_guard<std::mutex> lock(playerthreads.lock);

    for (auto &playerthread : playerthreads.items) {
        playerthread.finish();
    }

    deinitialize_python();
}

void Interpreter::deinitialize_python() {
    PyEval_RestoreThread(main_thread_state);
    print_debug << "Interpreter: Deinitialized Python" << std::endl;
}



py::api::object import_file(boost::filesystem::path filename) {
    auto importlib_machinery_module = py::import("importlib").attr("machinery");

    // Get and initialize SourceFileLoader
    auto SourceFileLoader_object = importlib_machinery_module.attr("SourceFileLoader");
    auto loader = SourceFileLoader_object("wrapper_functions", py::str(filename.string()));

    return loader.load_module();
}

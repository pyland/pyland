#include <boost/filesystem.hpp>
#include <boost/python.hpp>
#include <mutex>
#include "api.h"
#include "entitythread.h"
#include "interpreter.h"
#include "locks.h"
#include "make_unique.h"
#include "print_debug.h"
#include "thread_killer.h"

Interpreter::Interpreter(boost::filesystem::path function_wrappers) {

    // Leave locked forever to prove that it's only been locked once
    if (!initialized.try_lock()) {
        throw std::runtime_error("Interpreter initialized twice where only a single initialization supported");
    }

    main_thread_state = initialize_python();

    lock::Lockable<std::vector<std::unique_ptr<EntityThread>>> entitythreads;

    thread_killer = std::make_unique<ThreadKiller>(entitythreads);
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
    // and EntityThreads can be created without deadlocks
    PyEval_ReleaseLock();

    print_debug << "Interpreter: Released GIL " << std::endl;
    print_debug << "Interpreter: Interpreter created " << std::endl;
}

PyThreadState *Interpreter::initialize_python() {
    Py_Initialize();
    PyEval_InitThreads();

    print_debug << "Interpreter: Initialized Python" << std::endl;

    return PyThreadState_Get();
}

void Interpreter::register_entity(Entity entity) {
    std::lock_guard<std::mutex> lock(entitythreads.lock);

    entitythreads.items.push_back(std::move(std::make_unique<EntityThread>(this, entity)));
}

Interpreter::~Interpreter() {
    thread_killer->finish();
    print_debug << "Interpreter: Finished kill thread" << std::endl;

    // Not acutally needed; thread_killer is dead
    // However, this keeps the guarantees (locked while edited) safe,
    // so is good practice
    std::lock_guard<std::mutex> lock(entitythreads.lock);

    for (auto &entitythread : entitythreads.items) {
        entitythread->finish();
    }

    deinitialize_python();
}

void Interpreter::deinitialize_python() {
    PyEval_RestoreThread(main_thread_state);
    print_debug << "Interpreter: Deinitialized Python" << std::endl;
}



py::api::object Interpreter::import_file(boost::filesystem::path filename) {
    auto importlib_machinery_module = py::import("importlib").attr("machinery");

    // Get and initialize SourceFileLoader
    auto SourceFileLoader_object = importlib_machinery_module.attr("SourceFileLoader");
    auto loader = SourceFileLoader_object("wrapper_functions", py::str(filename.string()));

    return loader.attr("load_module")();
}

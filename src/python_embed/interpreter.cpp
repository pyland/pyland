#include <algorithm>
#include <atomic>
#include <boost/filesystem.hpp>
#include <boost/python.hpp>
#include <mutex>
#include <string>
#include "api.hpp"
#include "entitythread.hpp"
#include "interpreter.hpp"
#include "interpreter_context.hpp"
#include "locks.hpp"
#include "make_unique.hpp"
#include "print_debug.hpp"
#include "thread_killer.hpp"


// Initialize unset.
// WARNING: This is the only valid way to initialize this type.
std::atomic_flag Interpreter::initialized = ATOMIC_FLAG_INIT;

Interpreter::Interpreter(boost::filesystem::path function_wrappers):
    // WARNING:
    //     Using non-static member function in initialization list.
    //     This can be dangerous!
    interpreter_context(initialize_python()) {

        thread_killer = std::make_unique<ThreadKiller>(entitythreads, interpreter_context);
        print_debug << "Interpreter: Spawned Kill thread" << std::endl;

        // All Python errors should result in a Python traceback    
        try {
            // TODO: decide whether to set sys.path

            // Import to allow conversion of classes, but no need to keep module reference
            interpreter_context.import_file(function_wrappers);
        }
        catch (py::error_already_set) {
            PyErr_Print();
            throw std::runtime_error("Cannot spawn thread killer. Bailing early.");
        }

        // Release GIL; thread_killer can start killing now
        // and EntityThreads can be created without deadlocks
        PyEval_ReleaseLock();
        print_debug << "Interpreter: Interpreter created " << std::endl;
}

// WARNING:
//     Currently needs to be not use any member functions.
//     The only reason this is not static is to reflect the
//     design decision that multiple Interpreters could be
//     created at a later date.
//     This can be dangerous!
PyThreadState *Interpreter::initialize_python() {
    // Make sure it's a singleton
    if (initialized.test_and_set()) {
        throw std::runtime_error("Interpreter initialized twice where only a single initialization supported");
    }

    Py_Initialize();
    PyEval_InitThreads();

    print_debug << "Interpreter: Initialized Python" << std::endl;

    return PyThreadState_Get();
}

LockableEntityThread Interpreter::register_entity(Entity &entity) {
    std::lock_guard<std::mutex> lock(*entitythreads.lock);

    // Create thread and move to vector.
    auto new_entity = std::make_shared<EntityThread>(interpreter_context, entity);
    entitythreads.value.push_back(std::weak_ptr<EntityThread>(new_entity));

    // Return the new EntityThread to allow signals to the thread.
    // This means that EntityThread needs to be thread-safe for
    // all approved usages.
    return LockableEntityThread(new_entity, entitythreads.lock);
}

Interpreter::~Interpreter() {
    // Join killer
    thread_killer->finish();
    print_debug << "Interpreter: Finished kill thread" << std::endl;

    // Exorcise daemons
    {
        // Lock not acutally needed; thread_killer is dead
        // However, this keeps the guarantees (locked while edited) safe,
        // so is good practice
        std::lock_guard<std::mutex> lock(*entitythreads.lock);
        entitythreads.value.clear();
    }

    // Finished Python
    deinitialize_python();
}

void Interpreter::deinitialize_python() {
    PyEval_RestoreThread(interpreter_context.get_threadstate());
    print_debug << "Interpreter: Deinitialized Python" << std::endl;
}

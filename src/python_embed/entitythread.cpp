#include <boost/filesystem.hpp>
#include <boost/python.hpp>
#include <future>
#include <thread>
#include "entitythread.hpp"
#include "interpreter_context.hpp"
#include "locks.hpp"
#include "make_unique.hpp"
#include "print_debug.hpp"

// For PyThread_get_thread_ident
#include "pythread.h"

namespace py = boost::python;

///
/// A thread function running a player's daemon.
///
/// @param entity_object
///     Python object to pass to the bootstrapper, which has API calls passed to it.
///
/// @param thread_id_promise
///     Promise allowing the thread to asynchronously return the thread's id,
///     according to CPython.
///
/// @param bootstrapper_file
///     The Python file that can bootstrap the process, taking an entity, running
///     it's files and controling logic (such as handling asynchronous exceptions).
///
/// @param interpreter_context
///     The interpreter_context of the main interpreter, allowing creation of a new thread
///     by access of the interpreter's PyInterpreterState.
///
///     Also allows importing files.
///
void run_entity(std::shared_ptr<py::api::object> entity_object,
                std::promise<long> thread_id_promise,
                boost::filesystem::path bootstrapper_file,
                InterpreterContext interpreter_context) {

    print_debug << "run_entity: Starting" << std::endl;

    {
        lock::GIL lock_gil(interpreter_context, "EntityThread::EntityThread");

        //PyObject_Print(py::str("WE ARE FIGHTING DREEBAS").ptr(), stdout, 0);
        print_debug << std::endl;
        PyObject_Print(py::api::object(entity_object->attr("name")).ptr(), stdout, 0);
        print_debug << std::endl;
    }

    // Register thread with Python, to allow locking
    lock::ThreadState threadstate(interpreter_context);
    lock::ThreadGIL lock_thread(threadstate);

    print_debug << "run_entity: Stolen GIL" << std::endl;

    try {
        // Asynchronously return thread id to allow killing of this thread
        thread_id_promise.set_value(PyThread_get_thread_ident());

        // Get and run bootstrapper
        auto bootstrapper_module = interpreter_context.import_file(bootstrapper_file);
        bootstrapper_module.attr("start")(*entity_object);
    }
    catch (py::error_already_set &) {
        // TODO: catch and nicely handle error
        std::cout << "Thread died or was halted." << std::endl;
        PyErr_Print();
    }
    
    print_debug << "run_entity: Finished" << std::endl;
}

EntityThread::EntityThread(InterpreterContext interpreter_context, Entity &entity):
    entity(entity),
    previous_call_number(entity.call_number),
    interpreter_context(interpreter_context) {

        // To get thread_id
        std::promise<long> thread_id_promise;
        thread_id_future = thread_id_promise.get_future();

        // Wrap the object for Python.
        //
        // For implementation justifications, see
        // http://stackoverflow.com/questions/24477791
        {
            lock::GIL lock_gil(interpreter_context, "EntityThread::EntityThread");
            entity_object = std::make_shared<py::api::object>(boost::ref(entity));
        };

        thread = std::make_unique<std::thread>(
            run_entity,
            entity_object,
            std::move(thread_id_promise),
            // TODO: Extract into a more logical place
            boost::filesystem::path("python_embed/scripts/bootstrapper.py"),
            interpreter_context
        );
}

long EntityThread::get_thread_id() {
    // TODO: Make thread-safe, if needed
    if (thread_id_future.valid()) {
        thread_id = thread_id_future.get();
    }

    return thread_id;
}

void EntityThread::halt_soft(Signal signal) {
    print_debug << "Attempting to kill thread id " << get_thread_id() << "." << std::endl;

    PyThreadState_SetAsyncExc(get_thread_id(), PyExc_SystemError);
}

void EntityThread::halt_hard() {
    // TODO: everything!!!
    throw std::runtime_error("hard halting not implemented");
}

bool EntityThread::is_dirty() {
    return previous_call_number != entity.call_number;
}

void EntityThread::clean() {
    previous_call_number = entity.call_number;
}

void EntityThread::finish() {
    // TODO: implement clean destruction of EntityThreads
    throw std::runtime_error("EntityThread::finish not implemented");
}

EntityThread::~EntityThread() {
    finish();
    print_debug << "EntityThread destroyed" << std::endl;

    lock::GIL lock_gil(interpreter_context, "EntityThread::~EntityThread");

    if (!entity_object.unique()) {
        throw std::runtime_error("multiple references to entity_object on destruction");
    }

    entity_object.reset();
}

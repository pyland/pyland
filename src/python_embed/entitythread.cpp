#include "python_embed_headers.hpp"

#include <boost/filesystem/path.hpp>
#include <boost/python.hpp>
#include <boost/ref.hpp>
#include <future>
#include <glog/logging.h>
#include <glm/vec2.hpp>
#include <thread>

#include "api.hpp"
#include "entitythread.hpp"
#include "event_manager.hpp"
#include "interpreter_context.hpp"
#include "locks.hpp"
#include "make_unique.hpp"

// For PyThread_get_thread_ident
#include "pythread.h"

namespace py = boost::python;

LockableEntityThread::LockableEntityThread():
    lock::Lockable<std::shared_ptr<EntityThread>>() {}

LockableEntityThread::LockableEntityThread(std::shared_ptr<EntityThread> value):
    lock::Lockable<std::shared_ptr<EntityThread>>(value) {}

LockableEntityThread::LockableEntityThread(std::shared_ptr<EntityThread> value, std::shared_ptr<std::mutex> lock):
    lock::Lockable<std::shared_ptr<EntityThread>>(value, lock) {}

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
                InterpreterContext interpreter_context,
                std::map<EntityThread::Signal, PyObject *> signal_to_exception) {

    LOG(INFO) << "run_entity: Starting";

    bool waiting = true;

    // Register thread with Python, to allow locking
    lock::ThreadState threadstate(interpreter_context);

    std::unique_ptr<py::api::object> bootstrapper_module;

    {
        lock::ThreadGIL lock_thread(threadstate);

        LOG(INFO) << "run_entity: Stolen GIL";

        // Get and run bootstrapper
        bootstrapper_module = std::make_unique<py::api::object>(
            interpreter_context.import_file(bootstrapper_file)
        );

        // Asynchronously return thread id to allow killing of this thread
        //
        // WARNING:
        //     This causes subtle race conditions, as setting this requires
        //     the GIL and the thread killer takes the GIL.
        //
        //     BE CAREFUL.
        //
        thread_id_promise.set_value(PyThread_get_thread_ident());
    }

    while (true) {
        try {
            lock::ThreadGIL lock_thread(threadstate);

            bootstrapper_module->attr("start")(
                *entity_object,
                py::api::object(py::borrowed<>(signal_to_exception[EntityThread::Signal::RESTART])),
                py::api::object(py::borrowed<>(signal_to_exception[EntityThread::Signal::STOP])),
                py::api::object(py::borrowed<>(signal_to_exception[EntityThread::Signal::KILL])),
                waiting
            );
        }
        catch (py::error_already_set &) {

            lock::ThreadGIL lock_thread(threadstate);

            PyObject *type, *value, *traceback;
            PyErr_Fetch(&type, &value, &traceback);

            if (!type) {
                throw std::runtime_error("Unknown Python error");
            }

            if (PyErr_GivenExceptionMatches(signal_to_exception[EntityThread::Signal::RESTART], type)) {
                waiting = false;
                continue;
            }
            else if (PyErr_GivenExceptionMatches(signal_to_exception[EntityThread::Signal::STOP], type)) {
                // Just wait.
                waiting = true;
                continue;
            }
            else if (PyErr_GivenExceptionMatches(signal_to_exception[EntityThread::Signal::KILL], type)) {
                // We are done.
                LOG(INFO) << "Thread is killed";
                return;
            }
            else {
                LOG(WARNING) << "Python error in EntityThread, Python side.";
            }
        }
        waiting = true;
    }

    LOG(INFO) << "run_entity: Finished";
}

EntityThread::EntityThread(InterpreterContext interpreter_context, Entity &entity):
    entity(entity),
    previous_call_number(entity.call_number),
    interpreter_context(interpreter_context),

    Py_BaseAsyncException(make_base_async_exception(PyExc_BaseException, "__main__.BaseAsyncException")),

    signal_to_exception({
        {
            EntityThread::Signal::RESTART,
            make_base_async_exception(Py_BaseAsyncException, "__main__.BaseAsyncException_RESTART")
        }, {
            EntityThread::Signal::STOP,
            make_base_async_exception(Py_BaseAsyncException, "__main__.BaseAsyncException_STOP")
        }, {
            EntityThread::Signal::KILL,
            make_base_async_exception(Py_BaseAsyncException, "__main__.BaseAsyncException_KILL")
        }
    })

    {

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
            // TODO: Extract path into a more logical place
            boost::filesystem::path("python_embed/scripts/bootstrapper.py"),
            interpreter_context,
            signal_to_exception
        );
}

long EntityThread::get_thread_id() {
    if (thread_id_future.valid()) {
        thread_id = thread_id_future.get();
    }

    return thread_id;
}


PyObject *EntityThread::make_base_async_exception(PyObject *base, const char *name) {
    lock::GIL lock_gil(interpreter_context, "EntityThread::make_base_async_exception");

    return PyErr_NewException(name, base, nullptr);
}

void EntityThread::halt_soft(Signal signal) {
    auto thread_id = get_thread_id();

    lock::GIL lock_gil(interpreter_context, "EntityThread::halt_soft");

    PyThreadState_SetAsyncExc(thread_id, signal_to_exception[signal]);
}

void EntityThread::halt_hard() {
    // TODO: everything!!!
    throw std::runtime_error("hard halting not implemented");

    lock::GIL lock_gil(interpreter_context, "EntityThread::halt_hard");
}

bool EntityThread::is_dirty() {
    return previous_call_number != entity.call_number;
}

void EntityThread::clean() {
    previous_call_number = entity.call_number;
}

void EntityThread::finish() {
    // TODO: implement nagging
    halt_soft(Signal::KILL);
    thread->join();
}

EntityThread::~EntityThread() {
    finish();
    LOG(INFO) << "EntityThread destroyed";

    lock::GIL lock_gil(interpreter_context, "EntityThread::~EntityThread");

    if (!entity_object.unique()) {
        throw std::runtime_error("multiple references to entity_object on destruction");
    }

    for (auto signal_exception : signal_to_exception) {
        Py_DECREF(signal_exception.second);
    }
    Py_DECREF(Py_BaseAsyncException);

    entity_object.reset();
}

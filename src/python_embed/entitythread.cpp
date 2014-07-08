#include <boost/filesystem.hpp>
#include <boost/python.hpp>
#include <future>
#include <thread>
#include "interpreter.h"
#include "locks.h"
#include "make_unique.h"
#include "print_debug.h"
#include "pythread.h"

namespace py = boost::python;

void run_entity(py::api::object entity,
                std::promise<long> thread_id_promise,
                boost::filesystem::path bootstrapper_file,
                PyInterpreterState *main_interpreter_state) {

    print_debug << "run_entity: Starting" << std::endl;

    lock::ThreadState threadstate(main_interpreter_state);
    lock::ThreadGIL lock_thread(threadstate);

    print_debug << "run_entity: Stolen GIL" << std::endl;

    try {
        thread_id_promise.set_value(PyThread_get_thread_ident());

        auto bootstrapper_module = Interpreter::import_file(bootstrapper_file);
        bootstrapper_module.attr("start")(entity);
    }
    catch (py::error_already_set &) {
        // TODO: catch and nicely handle error

        std::cout << "Thread died or was halted." << std::endl;
        PyErr_Print();
    }
}

EntityThread::EntityThread(Interpreter *interpreter, Entity &entity):
    entity(entity), previous_call_number(entity.call_number) {

        // To get thread_id
        std::promise<long> thread_id_promise;
        thread_id_future = thread_id_promise.get_future();

        // This seems to be the easy compromise.
        // http://stackoverflow.com/questions/24477791
        py::api::object entity_object = [&entity] () {
            lock::GIL lock_gil;
            return py::api::object(boost::ref(entity));
        } ();

        thread = std::make_unique<std::thread>(
            run_entity,
            entity_object,
            std::move(thread_id_promise),
            // TODO: Extract into a more logical place
            boost::filesystem::path("python_embed/scripts/bootstrapper.py"),
            interpreter->main_thread_state->interp
        );
}

long EntityThread::get_thread_id() {
    if (thread_id_future.valid()) {
        thread_id = thread_id_future.get();
    }

    return thread_id;
}

void EntityThread::halt_soft() {
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

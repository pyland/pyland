#include <boost/python.hpp>
#include <future>
#include <thread>
#include "make_unique.h"
#include "playerthread.h"
#include "thread_killer.h"


namespace py = boost::python;

void run_player(py::api::object player,
                std::promise<int64_t> thread_id_promise) {

    print_debug << "run_player: Starting" << std::endl;

    ThreadState threadstate(main_interpreter_state);
    ThreadGIL lock_thread(threadstate);

    print_debug << "run_player: Stolen GIL" << std::endl;

    try {
        // Totally a hack. Remove ASA(R)P.
        auto py_thread_id = py::import("threading").attr("current_thread")().attr("ident");
        thread_id_promise.set_value(py::extract<int64_t>(py_thread_id));

        // TODO: bootstrapper
        auto bootstrapper_module = import_file(bootstrapper_file);

        // TODO: code!
        bootstrapper_module.attr("evaluate")(code);
    }
    catch (py::error_already_set &) {
        // TODO: catch and nicely handle error
        PyErr_Print();
    }
}

PlayerThread::PlayerThread(Player &player):
    player(player), previous_call_number(player.call_number) {

        // To get thread_id
        std::promise<int64_t> thread_id_promise;
        auto thread_id_future = thread_id_promise.get_future();

        // This seems to be the easy compromise.
        // http://stackoverflow.com/questions/24477791
        py::api::object player_object = [&player] () {
            GIL lock_gil;
            return py::api::object(boost::ref(player));
        } ();

        thread = std::make_unique<std::thread>(
            run_player,
            player_object,
            std::move(thread_id_promise)
        );

        // TODO: This could easily be asynchronous for
        // a much faster instantiation.
        thread_id = thread_id_future.wait();
}

bool PlayerThread::halt_soft() {
    print_debug << "Attempting to kill thread id " << playerthread.thread_id << "." << std::endl;

    PyThreadState_SetAsyncExc(thread_id, PyExc_SystemError);
}

bool PlayerThread::halt_hard() {
    // TODO: everything!!!
    throw std::runtime_error("hard halting not implemented");
}

bool PlayerThread::is_dirty() {
    return previous_call_number != player.call_number;
}

void PlayerThread::clean() {
    previous_call_number = player.call_number;
}

void PlayerThread::finish() {
    throw std::runtime_error("PlayerThread.finish() not implemented");
}

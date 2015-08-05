#include "python_embed_headers.hpp"

#include <boost/python.hpp>
#include <glog/logging.h>
#include <mutex>
#include "interpreter_context.hpp"
#include "locks.hpp"


namespace lock {

    //https://stackoverflow.com/questions/1418015/how-to-get-python-exception-text TODO: comment and cleanup this code
    std::string get_python_error_message()
    {
        using namespace boost::python;
        using namespace boost;

        PyObject *exc,*val,*tb;
        object formatted_list, formatted;
        PyErr_Fetch(&exc,&val,&tb);
        handle<> hexc(exc),hval(allow_null(val)),htb(allow_null(tb)); 
        object traceback(import("traceback"));
        if (!tb) {
            object format_exception_only(traceback.attr("format_exception_only"));
            formatted_list = format_exception_only(hexc,hval);
        } else {
            object format_exception(traceback.attr("format_exception"));
            formatted_list = format_exception(hexc,hval,htb);
        }
        formatted = str("\n").join(formatted_list);
        return extract<std::string>(formatted);
    }

    int GIL::i = 0;

    GIL::GIL(InterpreterContext interpreter_context, std::string name): name(name) {
        inst = i;
        ++i;

        VLOG(1) << inst << " Aquiring GIL lock  " << name;
        PyEval_RestoreThread(interpreter_context.get_threadstate());
        VLOG(1) << inst << " GIL lock aquired   " << name;
    }

    GIL::GIL(InterpreterContext interpreter_context): GIL(interpreter_context, "") {}

    GIL::~GIL() {
        VLOG(1) << inst << " Releasing GIL lock " << name;
        PyEval_SaveThread();
        VLOG(1) << inst << " GIL lock released  " << name;
    }


    ThreadGIL::ThreadGIL(ThreadState &threadstate) {
        VLOG(1) << " Aquiring Thread GIL lock";
        PyEval_RestoreThread(threadstate.get_threadstate());
        VLOG(1) << " Thread GIL lock aquired";
    }

    ThreadGIL::~ThreadGIL() {
        VLOG(1) << " Releasing Thread GIL lock";
        PyEval_SaveThread();
        VLOG(1) << " Thread GIL lock released";
    }

    ThreadGILRelease::ThreadGILRelease() {
        VLOG(1) << " Releasing Thread GIL lock";
        threadstate = PyEval_SaveThread();
        VLOG(1) << " Thread GIL lock Released";
    }

    ThreadGILRelease::~ThreadGILRelease() {
        VLOG(1) << " Reaquiring Thread GIL lock";
        PyEval_RestoreThread(threadstate);
        VLOG(1) << " Thread GIL lock reaquired";
    }


    ThreadState::ThreadState(InterpreterContext interpreter_context) {
        threadstate = PyThreadState_New(interpreter_context.get_interpreterstate());
    }

    ThreadState::~ThreadState() {
        {
            VLOG(1) << "ThreadState: Getting GIL and clearing ThreadState";

            PyGILState_STATE state = PyGILState_Ensure();
            PyThreadState_Clear(threadstate);
            PyGILState_Release(state);
        }
        PyThreadState_Delete(threadstate);
    }

    PyThreadState *ThreadState::get_threadstate() {
        return threadstate;
    }
}

#include "Python.h"
#include "python_embed_headers.hpp"

#include <boost/python.hpp>
#include <glog/logging.h>
#include <mutex>
#include "interpreter_context.hpp"
#include "locks.hpp"


namespace lock {
    int GIL::i = 0;

    GIL::GIL(InterpreterContext interpreter_context, std::string name): name(name) {
        inst = i;
        ++i;

        LOG(INFO) << inst << " Aquiring GIL lock  " << name;
        PyEval_RestoreThread(interpreter_context.get_threadstate());
        LOG(INFO) << inst << " GIL lock aquired   " << name;
    }

    GIL::GIL(InterpreterContext interpreter_context): GIL(interpreter_context, "") {}

    GIL::~GIL() {
        LOG(INFO) << inst << " Releasing GIL lock " << name;
        PyEval_SaveThread();
        LOG(INFO) << inst << " GIL lock released  " << name;
    }


    ThreadGIL::ThreadGIL(ThreadState &threadstate) {
        LOG(INFO) << " Aquiring Thread GIL lock";
        PyEval_RestoreThread(threadstate.get_threadstate());
        LOG(INFO) << " Thread GIL lock aquired";
    }

    ThreadGIL::~ThreadGIL() {
        LOG(INFO) << " Releasing Thread GIL lock";
        PyEval_SaveThread();
        LOG(INFO) << " Thread GIL lock released";
    }


    ThreadState::ThreadState(InterpreterContext interpreter_context) {
        threadstate = PyThreadState_New(interpreter_context.get_interpreterstate());
    }

    ThreadState::~ThreadState() {   
        {
            LOG(INFO) << "ThreadState: Getting GIL and clearing ThreadState";

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

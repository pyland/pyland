#include "python_embed_headers.hpp"

#include <boost/python.hpp>
#include <mutex>
#include "interpreter_context.hpp"
#include "locks.hpp"
#include "print_debug.hpp"


namespace lock {
    int GIL::i = 0;

    GIL::GIL(InterpreterContext interpreter_context, std::string name): name(name) {
        inst = i;
        ++i;

        print_debug << inst << " Aquiring GIL lock  " << name  << std::endl;
        PyEval_RestoreThread(interpreter_context.get_threadstate());
        print_debug << inst << " GIL lock aquired   " << name  << std::endl;
    }

    GIL::GIL(InterpreterContext interpreter_context): GIL(interpreter_context, "") {}

    GIL::~GIL() {
        print_debug << inst << " Releasing GIL lock " << name  << std::endl;
        PyEval_SaveThread();
        print_debug << inst << " GIL lock released  " << name  << std::endl;
    }


    ThreadGIL::ThreadGIL(ThreadState &threadstate) {
        PyEval_RestoreThread(threadstate.get_threadstate());
    }

    ThreadGIL::~ThreadGIL() {
        PyEval_SaveThread();
    }


    ThreadState::ThreadState(InterpreterContext interpreter_context) {
        threadstate = PyThreadState_New(interpreter_context.get_interpreterstate());
    }

    ThreadState::~ThreadState() {   
        {
            print_debug << "ThreadState: Getting GIL and clearing ThreadState" << std::endl;

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

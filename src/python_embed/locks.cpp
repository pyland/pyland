#include <boost/python.hpp>
#include <mutex>
#include "locks.h"
#include "print_debug.h"

namespace lock {
    int GIL::i = 0;
    std::mutex GIL::global_lock;

    GIL::GIL(std::string name): name(name) {
        inst = i;
        ++i;
        print_debug << inst << " Aquiring GIL      " << name  << std::endl;

        global_lock.lock();
        print_debug << inst << " GIL lock aquired  " << name  << std::endl;
        gilstate = PyGILState_Ensure();
        //PyEval_AcquireLock();

        print_debug << inst << " GIL aquired       " << name  << std::endl;
    }

    GIL::GIL(): GIL("") {};

    GIL::~GIL() {
        print_debug << inst << " Releasing GIL     " << name  << std::endl;

        //PyEval_ReleaseLock();
        PyGILState_Release(gilstate);
        print_debug << inst << " GIL released      " << name  << std::endl;
        global_lock.unlock();

        print_debug << inst << " GIL lock released " << name  << std::endl;
    }


    ThreadGIL::ThreadGIL(ThreadState &threadstate) {
        PyEval_RestoreThread(threadstate.get_threadstate());
    }

    ThreadGIL::~ThreadGIL() {
        PyEval_SaveThread();
    }


    ThreadState::ThreadState(PyInterpreterState *interpreter_state) {
        threadstate = PyThreadState_New(interpreter_state);
    }

    ThreadState::~ThreadState() {   
        {
            print_debug << "ThreadState: Getting GIL and clearing ThreadState" << std::endl;
            GIL lock_gil;
            PyThreadState_Clear(threadstate);
        }
        PyThreadState_Delete(threadstate);
    }

    PyThreadState *ThreadState::get_threadstate() {
        return threadstate;
    }
};

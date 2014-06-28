#include <boost/python.hpp>
#include "debug.h"
#include "locks.h"

int GIL::i = 0;

GIL::GIL() {
	inst = i;
	++i;
	print_debug << inst << " Aquiring GIL" << std::endl;
	gilstate = PyGILState_Ensure();
	// PyEval_AcquireLock();
	print_debug << inst << " GIL aquired: " << PyGILState_Check() << std::endl;
}

GIL::~GIL() {
	print_debug << inst << " Releasing GIL" << std::endl;
	// PyEval_ReleaseLock();
	PyGILState_Release(gilstate);
	print_debug << inst << " GIL released" << std::endl;
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
	GIL lock_gil;
    PyThreadState_Delete(threadstate);
}

PyThreadState *ThreadState::get_threadstate() {
	return threadstate;
}
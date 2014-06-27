GIL::GIL() {
	gilstate = PyGILState_Ensure();
}

GIL::~GIL() {
	PyGILState_Release(gilstate);
}


ThreadGIL::ThreadGIL(ThreadStatethreadstate) {
	PyEval_RestoreThread(threadstate.get_threadstate());
}

ThreadGIL::~ThreadGIL() {
	PyEval_SaveThread();
}


ThreadState::ThreadState(PyInterpreterState *interpreter_state) {
	threadstate = PyThreadState_New(main_interpreter_state);
}

ThreadState::~ThreadState() {
	GIL lock_gil;
    PyThreadState_Delete(threadstate);
}

PyThreadState *ThreadState::get_threadstate() {
	return threadstate;
}
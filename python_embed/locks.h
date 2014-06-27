///
/// A RAII lock for the GIL. Usage:
///
///     {
///         GIL lock_gil;
///         stuff();
///     }
///
class GIL {
    public:
        GIL();
        ~GIL();

    private:
        GIL(const GIL&);
        PyGILState_STATE gilstate;
};


///
/// A RAII lock for PyThreadState GIL grabbing. Usage:
///
///     {
///         ThreadGIL lock_thread(threadstate);
///         stuff();
///     }
///
class ThreadGIL {
    public:
        ThreadGIL(ThreadState threadstate);
        ~ThreadGIL();

    private:
        ThreadGIL(const ThreadGIL&);
};

///
/// A RAII lock for PyThreadState creation. Usage:
///
///     {
///         ThreadState threadstate(interpreter_state);
///         stuff(thredstate.get_threadstate());
///     }
///
class ThreadState {
    public:
        ThreadState();
        ~ThreadState();
        PyThreadState *get_threadstate();

    private:
        ThreadState(const ThreadState&);
        PyThreadState *threadstate;
};

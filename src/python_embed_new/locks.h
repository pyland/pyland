#ifndef LOCKS_H
#define LOCKS_H

#include <boost/python.hpp>
#include <mutex>

namespace lock {
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

            static int i;
            int inst;

        private:
            GIL(const GIL&);
            PyGILState_STATE gilstate;
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
            ThreadState(PyInterpreterState *interpreter_state);
            ~ThreadState();
            PyThreadState *get_threadstate();

        private:
            ThreadState(const ThreadState&);
            PyThreadState *threadstate;
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
            ThreadGIL(ThreadState &);
            ~ThreadGIL();

        private:
            ThreadGIL(const ThreadGIL&);
    };


    template <class T>
    struct Lockable {
        T items;
        std::mutex lock;
    };
};

#endif

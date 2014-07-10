#ifndef LOCKS_H
#define LOCKS_H

#include <boost/python.hpp>
#include <mutex>
#include "print_debug.h"

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
            GIL(std::string name);
            GIL();
            ~GIL();

            static int i;
            int inst;

        private:
            GIL(const GIL&);
            PyGILState_STATE gilstate;
            static std::mutex global_lock;
            std::string name;
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
    class Lockable {
        public:
            Lockable(): value(), lock() {};
            Lockable(T value): value(value), lock() {};
            ~Lockable() {};

            T value;
            std::mutex lock;
    };

    template<class T, class... Args>
    Lockable<T> make_lockable(Args &&... args) {
        return Lockable<T>(new T(std::forward<Args>(args)...));
    };

};

#endif

#ifndef THREAD_KILLER_H
#define THREAD_KILLER_H

#include "python_embed_headers.hpp"

#include <memory>
#include <mutex>
#include <vector>
#include "entitythread.hpp"
#include "locks.hpp"

///
/// Wrapper that keeps a thread to kill threads contained inside
/// the passed lockable vector. If the contained EntityThread
/// objects don't call API functions often enough, they will
/// be killed by this thread.
///
class ThreadKiller {
    public:
        ///
        /// Spawn a thread to kill threads contained inside
        /// the passed lockable vector. If the contained EntityThread
        /// objects don't call API functions often enough, they will
        /// be killed by this thread.
        ///
        /// Modifications on the vector happen when locked,
        /// and it is assumed the same is true for elsewhere.
        ///
        /// @param entitythreads Lockable container of all threads
        ///                      meant to be killed.
        ///
        /// @param interpreter_context
        ///     An interpreter context to lock the GIL on.
        ///     The GIL is locked on the main thread.
        ///
        ThreadKiller(EntityThreads &entitythreads);

        ///
        /// Halt the thread. Waits untill the thread has finished.
        ///
        void finish();

    private:
        ///
        /// Mutex for thread to sleep on, allowing interruptable thread sleeps.
        ///
        std::timed_mutex kill_thread_finish_signal;

        ///
        /// Thread.
        ///
        std::thread thread;
};

#endif

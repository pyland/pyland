#ifndef THREAD_KILLER_H
#define THREAD_KILLER_H

#include <mutex>
#include "entitythread.h"
#include "locks.h"


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
        ThreadKiller(lock::Lockable<std::vector<std::unique_ptr<EntityThread>>> &entitythreads);

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

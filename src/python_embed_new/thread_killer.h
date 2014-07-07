#include "lockable.h"

class ThreadKiller {
    public:
        ///
        /// Spawn a thread to kill threads contained inside
        /// the passed lockable vector. If the contained PlayerThread
        /// objects don't call API functions often enough, they will
        /// be killed by this thread.
        ///
        /// Modifications on the vector happen when locked,
        /// and it is assumed the same is true for elsewhere.
        ///
        /// @param playerthreads Lockable container of all threads
        ///                      meant to be killed.
        ///
        ThreadKiller(Lockable<std::vector<PlayerThread>> &playerthreads);

        ///
        /// Halt the thread. Waits untill the thread has finished.
        ///
        void finish();

    private:
        ///
        /// Mutex for thread to sleep on, allowing interruptable thread sleeps.
        ///
        std::mutex kill_thread_finish_signal;

        ///
        /// Thread.
        ///
        std::thread thread;
};

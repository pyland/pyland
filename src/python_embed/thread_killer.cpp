#include <chrono>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>
#include "entitythread.hpp"
#include "locks.hpp"
#include "print_debug.hpp"
#include "thread_killer.hpp"

///
/// Wrapper function for std::timed_mutex::try_lock_for which works around bugs in
/// the implementations in certain compilers.
///
/// Waiting can be more, but not less than, the required period, assuming that
/// the user's clock is working as expected and that the lock is not acqured.
///
/// @param lock
///     Mutex to wait on.
///
/// @param time_period
///     Minimal length of time to wait for.
///
/// @return
///     Whether the lock has been acquired.
///
bool try_lock_for_busywait(std::timed_mutex &lock, std::chrono::nanoseconds time_period) {
    auto end = std::chrono::system_clock::now() + time_period;

    while (true) {
        // If lock is taken, return true. Otherwise, try to delay
        if (lock.try_lock_for(end - std::chrono::system_clock::now())) {
            return true;
        }

        // If delay has passed, hasn't been locked so return false
        if (end <= std::chrono::system_clock::now()) {
            return false;
        }

        // Make sure not a busy wait on broken compilers
        std::this_thread::sleep_for(time_period / 100);
    }
}

///
/// A thread to kill threads contained inside the passed lockable vector.
/// If the contained EntityThread objects don't call API functions often
/// enough, they will be killed by this thread.
///
/// @param finish_signal
///     A mutex to wait on that will be unlocked when the thread should finish.
///
/// @param entitythreads
///     Reference to the entitythreads to police.
///
/// @warning
///     Arguments must be passed in a std::reference_wrapper to avoid copies.
///
/// @warning
///     The lifetimes of finish_signal and entitythreads are not obvious.
///     A fair amount of effort was spent making sure this was safe. All
///     usage should keep this in mind.
///
void thread_killer(std::timed_mutex &finish_signal,
                   lock::Lockable<std::vector<std::unique_ptr<EntityThread>>> &entitythreads) {

    while (true) {
        // Nonbloking sleep; allows safe quit
        if (try_lock_for_busywait(finish_signal, std::chrono::milliseconds(10000))) {
            break;
        }

        print_debug << "Kill thread woke up" << std::endl;
        print_debug << "111" << entitythreads.value.size() << std::endl;

        std::lock_guard<std::mutex> lock(entitythreads.lock);

        // Go through the available entitythread objects and kill those that
        // haven't had an API call.
        for (auto &entitythread : entitythreads.value) {
            if (!entitythread->is_dirty()) {
                print_debug << "Killing thread!" << std::endl;
                lock::GIL lock_gil("thread_killer");
                entitythread->halt_soft();
            }
            entitythread->clean();
        }
    }

    print_debug << "Finished kill thread" << std::endl;
}

ThreadKiller::ThreadKiller(lock::Lockable<std::vector<std::unique_ptr<EntityThread>>> &entitythreads) {
        // Lock now to prevent early exit
        kill_thread_finish_signal.lock();

        thread = std::thread(
            thread_killer,
            std::ref(kill_thread_finish_signal),
            std::ref(entitythreads)
        );

        print_debug << "main: Spawned Kill thread" << std::endl;
}

void ThreadKiller::finish() {    
    print_debug << "main: Stopping Kill thread" << std::endl;

    // Signal that the thread can quit
    kill_thread_finish_signal.unlock();
    thread.join();
}

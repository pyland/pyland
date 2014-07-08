#include <chrono>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>
#include "entitythread.h"
#include "locks.h"
#include "print_debug.h"
#include "thread_killer.h"


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

void thread_killer(std::timed_mutex &finish_signal,
                   lock::Lockable<std::vector<std::unique_ptr<EntityThread>>> &entitythreads) {

    while (true) {
        // Nonbloking sleep; allows safe quit
        if (try_lock_for_busywait(finish_signal, std::chrono::milliseconds(1000))) {
            break;
        }

        print_debug << "Kill thread woke up" << std::endl;
        std::lock_guard<std::mutex> lock(entitythreads.lock);

        // Go through the available entitythread objects and kill those that
        // haven't had an API call.
        for (auto &entitythread : entitythreads.items) {
            if (!entitythread->is_dirty()) {
                lock::GIL lock_gil;
                entitythread->halt_soft();
            }
            entitythread->clean();
        }
    }

    print_debug << "Finished kill thread" << std::endl;
}

// TODO: Check lifetimes are valid for entitythreads
ThreadKiller::ThreadKiller(lock::Lockable<std::vector<std::unique_ptr<EntityThread>>> &entitythreads) {

		kill_thread_finish_signal.lock();

		std::thread thread2(
			thread_killer,
			std::ref(kill_thread_finish_signal),
			std::ref(entitythreads)
		);

		print_debug << "main: Spawned Kill thread" << std::endl;
}

void ThreadKiller::finish() {
    kill_thread_finish_signal.unlock();
    thread.join();
}

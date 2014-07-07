#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include "interpreter.h"
#include "locks.h"

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
                   Lockable<std::vector<PlayerThread>> &playerthreads) {

    while (true) {
        // Nonbloking sleep; allows safe quit
        if (try_lock_for_busywait(finish_signal, std::chrono::milliseconds(1000))) {
            break;
        }

        print_debug << "Kill thread woke up" << std::endl;
        std::lock_guard<std::mutex> lock(playerthreads.lock);

        // Go through the available playerthread objects and kill those that
        // haven't had an API call.
        for (auto &playerthread : playerthreads.items) {
            if (!playerthread.is_dirty()) {
                GIL lock_gil;
                playerthread.halt_soft();
            }
            playerthread.set_clean();
        }
    }

    print_debug << "Finished kill thread" << std::endl;
}

ThreadKiller::ThreadKiller(Lockable<std::vector<PlayerThread> playerthreads):
	playerthreads(playerthreads) {

		kill_thread_finish_signal.lock();

		thread = std::move(std::thread(
			thread_killer,
			std::ref(kill_thread_finish_signal),
			std::ref(playerthreads)
		));

		print_debug << "main: Spawned Kill thread" << std::endl;
}

void finish() {
    kill_thread_finish_signal.unlock();
    thread.join();
}

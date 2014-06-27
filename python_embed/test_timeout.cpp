#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <ctime>

std::timed_mutex lock;

void waitsometime() {
	std::cout << 1 << std::endl;

	auto end = std::chrono::system_clock::now() + std::chrono::milliseconds(10000);

	while (true) {
		lock.try_lock_for(end - std::chrono::system_clock::now());
		if (end <= std::chrono::system_clock::now()) { break; }
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	std::cout << 2 << std::endl;
}

int main(int, char **) {
	lock.lock();
	std::thread mythread(waitsometime);
	mythread.join();
}

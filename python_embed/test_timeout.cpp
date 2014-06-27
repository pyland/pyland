#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

std::timed_mutex lock;

void waitsometime() {
	std::cout << 1 << std::endl;
	lock.try_lock_for(std::chrono::milliseconds(10000));
	std::cout << 2 << std::endl;
}

int main(int, char **) {
	lock.lock();
	std::thread mythread(waitsometime);
	mythread.join();
}

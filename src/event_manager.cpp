#include "event_manager.hpp"

#include <mutex>
#include <thread>


EventManager::EventManager() {
}

EventManager::~EventManager() {
}

void EventManager::register_event() {
    // a std::lock_guard to manage locking in an exception-safe manner
    std::lock_guard<std::mutex> lock(queue_mutex);

    //Lock released when this lock_guard goes out of scope
}

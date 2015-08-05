#include "python_embed_headers.hpp"

#include <algorithm>
#include <chrono>
#include <functional>
#include <glog/logging.h>
#include <list>
#include <mutex>
#include <ostream>
#include <ratio>

#include "event_manager.hpp"
#include "game_time.hpp"
#include "locks.hpp"


EventManager::EventManager(): enabled(true) {
    // Allocate on the heap so that we can swap the curr_frame and next_frame
    curr_frame_queue = new std::list<std::function<void ()>>();
    next_frame_queue = new std::list<std::function<void ()>>();
}

EventManager::~EventManager() {
    // Free these
    delete curr_frame_queue;
    delete next_frame_queue;
}

EventManager *EventManager::get_instance(){
    // Lazy instantiation of the global instance
    static EventManager global_instance;

    return &global_instance;
}

void EventManager::flush_and_disable(InterpreterContext &interpreter_context) {
    //
    // This will clear both lists out. Now, if another thread tries
    // to add something but blocks before getting a lock (but is stil
    // in an add_event function), then, once this method completes,
    // that event would still be added to the queue.
    //
    // The intention of this function is to be used once all the
    // threads that are putting data onto the event queues are finished.
    // Essentially, it is run after maps are unloaded and we are
    // preparing for a new map.
    //

    // The lock_guard is exception safe and releases the mutex when
    // it goes out of scope. So we introduce scope here to release
    // the mutex
    {
        //lock the Python GIL. Automatically unlocks it on destruction (when it goes out of scope).
        //neccesary for when there are python callbacks on the event queue. As they GIL needs to be locked when the are destructed.
        lock::GIL lock_gil(interpreter_context, "EventManager::process_events");
        //Lock the lists
        std::lock_guard<std::mutex> lock(queue_mutex);

        enabled = false;

        //Clear both lists
        curr_frame_queue->clear();
        next_frame_queue->clear();

    }
    // Lock released
}

void EventManager::process_events(InterpreterContext &interpreter_context) {
    // We need to process all the events in the queue
    // Problem is that, when events are being processed, they can add
    // further events. If we have the lock on the lock_guard in the
    // process loop when this happens, then we cannot add the new event
    // and so the thread blocks and the lock is never released
    //
    // So, we need to extract an event from the queue, lock the queue
    // to do this. We then release the lock and process the event.
    // We then repeat the process until the entire queue is finished
    //
    while (true) {
        //lock the Python GIL. Automatically unlocks it on destruction (when it goes out of scope).
        //neccesary for when there are python callbacks on the event queue. As they GIL needs to be locked when the are run and destructed.
        lock::GIL lock_gil(interpreter_context, "EventManager::process_events");
        //The callback function we need to process
        std::function<void ()> func;

        //The lock_guard is exception safe and releases the mutex when
        //it goes out of scope. So we introduce scope here to release
        //the mutex once we've pulled the event from it.
        {
            //Lock the list
            std::lock_guard<std::mutex> lock(queue_mutex);

            //If the queue is empty, exit this processing
            if(curr_frame_queue->empty()) {
                //This is safe as we have the lock
                std::swap(curr_frame_queue, next_frame_queue);
                break;
            }

            //Get the first element in the list
            //We've locked the queue so it will definitely have an item
            func = curr_frame_queue->front();
            curr_frame_queue->pop_front();
        } // Lock released

        //Dispatch the callback
        if(func) {
            try {
                func();
            } catch(boost::python::error_already_set &) { //catch any python erros
                std::string msg = lock::get_python_error_message(); //get python error message
                LOG(INFO) << msg;  //log it and continue running
            }
        }
        else {
            LOG(ERROR) << "ERROR in event_manager.cpp in processing, no function";
        }
    }
}
void EventManager::add_event(std::function<void ()> func) {
    // Manages locking in an exception-safe manner
    // Lock released when this lock_guard goes out of scope
    std::lock_guard<std::mutex> lock(queue_mutex);

    if (!enabled) { return; }

    //Add it to the queue
    curr_frame_queue->push_back(func);
}

void EventManager::add_event_next_frame(std::function<void ()> func) {
    // Manages locking in an exception-safe manner
    // Lock released when this lock_guard goes out of scope
    std::lock_guard<std::mutex> lock(queue_mutex);

    if (!enabled) { return; }

    //Add it to the queue
    next_frame_queue->push_back(func);
}

void EventManager::reenable() { enabled = true; }

void EventManager::add_timed_event(GameTime::duration duration, std::function<bool (float)> func) {
    // This needs to be thread-safe, so wrap it in an event.
    // Also, this holds the initialisation, so as to keep it static
    // between all of the events.
    add_event([this, duration, func] () {
        auto start_time = time.time();

        // Convert a timed callback to a void lambda by creating a wrapper
        // that keeps track of the completion and deals with re-registering.
        static std::function<void (GameTime::duration,          std::function<bool (float)>,      GameTime::time_point)> callback =
               [&]                (GameTime::duration duration, std::function<bool (float)> func, GameTime::time_point start_time) {

            auto completion = time.time() - start_time;

            // Don't allow finite polling speed to allow > 100% completion.
            float fraction_complete(float(std::min(completion / duration, 1.0)));

            if (func(fraction_complete) && fraction_complete < 1.0) {
                // Repeat if the callback wishes and the event isn't complete.
                add_event_next_frame(std::bind(callback, duration, func, start_time));
            }
        };

        add_event(std::bind(callback, duration, func, start_time));
    });
}

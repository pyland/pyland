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

EventManager &EventManager::get_instance() {
    // Lazy instantiation of the global instance
    static EventManager global_instance;

    return global_instance;
}

void EventManager::flush_and_disable() {
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
        //Lock the lists
        std::lock_guard<std::mutex> lock(queue_mutex);

        enabled = false;

        //Clear both lists
        curr_frame_queue->clear();
        next_frame_queue->clear();

    }
    // Lock released
}

/// A quicky class to handle Global intepreter locks for python events on the event queueu
/// need to have a better way of doing this.
/// Esentially, in a nutshell, there was a problem when lambda functions containing python code were being passed onto the event 
/// queue.  In the while loop, the functions were being destructed as they fell out of scope, while the engine wasn't holding
/// onto the GIL, causing undedertimned behaviour, This hack is a quick workaround that https://stackoverflow.com/questions/28220003/segfault-from-dict-destructor
/// I will speak to Alex about this to determine what the best thing would be to do, my current thought's are:
///    - have a special add event for python code for the event quueue
///    - in process_events, check if it one of those, and if it is ensure GIL is held when object falls out of scope.
/// TODO: Speak to Alex.
class gil_lock
{
public:
  gil_lock()  { state_ = PyGILState_Ensure(); }
  ~gil_lock() { PyGILState_Release(state_);   }
private:
  PyGILState_STATE state_;
};

void EventManager::process_events() {
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
        LOG(INFO) << "locating segfault";
        gil_lock lock; //see comments of class defined above
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
            func();
        }
        else {
            LOG(ERROR) << "ERROR in event_manager.cpp in processing, no function";
        }
        LOG(INFO) << "Done Done the thing!!!";
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

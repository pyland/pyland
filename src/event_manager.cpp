#include <functional>
#include <glog/logging.h>
#include <iostream>
#include <list>
#include <mutex>
#include <thread>

#include "event_manager.hpp"
#include "game_time.hpp"


EventManager::EventManager() {
    //Allocate on the heap so that we can swap the curr_frame and next_frame
    curr_frame_queue = new std::list<std::function<void ()>>();
    next_frame_queue = new std::list<std::function<void ()>>();
}

EventManager::~EventManager() {
    //Free these
    delete curr_frame_queue;
    delete next_frame_queue;
}

EventManager& EventManager::get_instance() {
    //:Lazy instantiation of the global instance
    static EventManager global_instance;
    
    return global_instance;
}

void EventManager::process_events() {
    //We need to process all the events in the queue
    //Problem is that, when events are being processed, they can add
    //further events. If we have the lock on the lock_guard in the
    //process loop when this happens, then we cannot add the new event
    //and so the thread blocks and the lock is never released
    //
    //So, we need to extract an event from the queue, lock the queue
    //to do this. We then release the lock and process the event.
    //We then repeat the process until the entire queue is finished
    //
    while(true) {
        //The callback function we need to process
        std::function<void ()> func;

        //The lock_guard is exception safe and releases the mutex when
        //it goes out of scope. So we introduce scope here to release
        //the mutex once we've pulled the event from it.
        {
            //Lock the list
            std::lock_guard<std::mutex> lock(queue_mutex);

            //If the queue is empty, exit this processing
            //TODO, consider allowing only x number of events to be processed to stop infinite loops
            if(curr_frame_queue->empty()) {
                //We swap the current_frame and the next_frame
                //pointers so that the next_frame becomes the
                //curr_frame.
                //This is fine as we have the lock and only this event
                //manager instance has the pointer so nobody can add
                //items to the curr_frame queue whilst we do this
                auto temp = curr_frame_queue;
                curr_frame_queue = next_frame_queue;
                next_frame_queue = temp;

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
    }
}
void EventManager::add_event(std::function<void ()> func) {
    // Manages locking in an exception-safe manner
    // Lock released when this lock_guard goes out of scope
    std::lock_guard<std::mutex> lock(queue_mutex);

    //Add it to the queue
    curr_frame_queue->push_back(func);
}

void EventManager::add_timed_event(GameTime::duration duration, std::function<bool (double)> func) {
    GameTime::time_point start_time = time.time();

    add_timed_event(duration, func, start_time);
}
void EventManager::add_timed_event(GameTime::duration duration,
                                   std::function<bool (double)> func,
                                   GameTime::time_point start_time) {

    // Manages locking in an exception-safe manner
    // Lock released when this lock_guard goes out of scope
    std::lock_guard<std::mutex> lock(queue_mutex);

    // Convert a timed callback to a void lambda
    // We do this by creating a void wrapper lambda and use
    // variable capture to capture the data we need
    next_frame_queue->push_back([this, duration, func, start_time] () {

        //Calculate the total time since the initial start time
        GameTime::time_point curr_time = time.time();
        auto curr_interval(curr_time - start_time);
        
        // Calculate how far to completion we are.
        // curr_interval is in milliseconds so convert to seconds
        double completion = curr_interval / duration;

        //If the result is true, put it on the next frame queue
        bool result = func(completion);
        if(result) {
            add_timed_event(duration, func, start_time);
        } //else, don't need to do anything
    });
}

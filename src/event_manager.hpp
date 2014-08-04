#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <functional>
#include <list>
#include <mutex>
#include <thread>

#include "game_time.hpp"

///
/// The event manager class. This is a thread-safe
/// implementation.which uses the singleton pattern.
///
class EventManager {
    ///
    /// The mutex to control access to the queues
    ///
    std::mutex queue_mutex;


    ///
    ///The queue for lambdas to be dealt with in this frame
    /// We use a list as the iterator remains valid if we add and
    /// remove items from this list
    ///
    /// We use pointers for the queue so that we can perform a swap at
    /// the end of processing the curr frame queue in the current
    /// frame.
    /// That is, we swap the curr_frame_queue and next_frame_queue
    /// pointers so that the next frame becomes the curr frame. Like
    /// double-buffering in graphics.
    ///
    std::list<std::function<void ()>>* curr_frame_queue;

    ///
    ///The queue for lambdas to be dealt with in the next frame. Read
    ///curr_frame_queue's comment.
    ///
    std::list<std::function<void()>>* next_frame_queue;

    ///
    /// Add an event with a time duration to run for. e.g. a timer
    /// This is wrapped in a normal event (like one passed to
    /// add_event) internally and internally managed.
    ///
    /// @param duration The time duration to run the event for 
    /// @param func a boolean return and double argument
    /// lambda. Return true to keep it in the queue and return false
    /// to remove it. The double is intended to be a percentage of
    /// completion. 0.0 is 0% and 1.0 is 100%. This is calculated
    /// based on the duration, the current time and the time the event was added to the
    /// queue 
    /// @param percentage the percentage of completion
    ///

    void add_timed_event(GameTime::duration duration,
                         std::function<bool (double)> func,
                         GameTime::time_point start_time);

    EventManager();
    ~EventManager();
public:
    ///
    /// This deals with keeping track of the game's time,
    /// and allowing the speed of time to be varied.
    ///
    GameTime time;

    ///
    /// Getter for the main global event manager.
    /// @return a reference to the global event manager
    ///
    static EventManager& get_instance();
    ///
    /// Add an event to the event manager
    ///
    /// @param func This is a void argument, void return lambda
    /// function which will be called by the event manger when the event is
    /// processed in the queue.
    ///
    void add_event(std::function<void ()>  func);
    void add_event_next_frame(std::function<void ()>  func);

    ///
    /// Add an event with a time duration to run for. e.g. a timer
    /// This is wrapped in a normal event (like one passed to
    /// add_event) internally and internally managed.
    ///
    /// @param duration The time duration to run the event for
    /// @param func a boolean return and double argument
    /// lambda. Return true to keep it in the queue and return false
    /// to remove it. The double is intended to be a percentage of
    /// completion. 0.0 is 0% and 1.0 is 100%. This is calculated
    /// based on the duration, the current time and the time the event was added to the
    /// queue 
    void add_timed_event(GameTime::duration duration, std::function<bool (double)> func);

    ///
    /// Processes all events in the current frame queue
    ///
    void process_events();

};

#endif

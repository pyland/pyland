#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <functional>
#include <list>
#include <mutex>

#include "game_time.hpp"

class InterpreterContext;

///
/// The event manager class. This is a thread-safe
/// implementation.which uses the singleton pattern.
///
class EventManager {

private:

    EventManager();
    ~EventManager();
    
    EventManager(EventManager const&) = delete;
    void operator=(EventManager const&) = delete;

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
    /// Whether events added to the queue are listened to.
    /// When false, they are silently ignored.
    ///
    bool enabled;

public:
    ///
    /// This deals with keeping track of the game's time,
    /// and allowing the speed of time to be varied.
    ///
    GameTime time;

    ///
    /// Cleans out the current and next frame event queues.
    ///
    void flush();

    ///
    /// Cleans out the current and next frame event queues.
    /// Disables the queue, so registered events are silently ignored.
    ///
    /// @see reenable
    ///
    void flush_and_disable(InterpreterContext &interpreter_context);

    ///
    /// Reenable the queue after a call to flush_and_disable.
    ///
    /// @see flush_and_disable
    ///
    void reenable();

    ///
    /// Getter for the main global event manager.
    /// @return a reference to the global event manager
    ///
    static EventManager* get_instance();

    ///
    /// Add an event to the event manager. If the event loop is running,
    /// is will be added to the end of the current queue and also run
    /// in this event frame. Otherwise it will be delayed until the
    /// circumstance occurs.
    ///
    /// The callback will be silently ignored if the event manager is disabled.
    ///
    /// @param func
    ///     A callback with no arguments and no return, to be
    ///     run on the current or upcomming frame.
    ///
    /// @see add_event_next_frame
    ///
    void add_event(std::function<void ()> func);

    ///
    /// Add an event to the event manager to be called after this event
    /// frame is finished. This is typically not what is wanted, and exists
    /// only to allow animations and other self-repeating callbacks.
    ///
    /// The callback will be silently ignored if the event manager is disabled.
    ///
    /// @param func
    ///     A callback with no arguments and no return, to be
    ///     run on the frame after this one.
    ///
    /// @note
    ///     Many event frames can occur between each rendered frame,
    ///     so don't expect a consistent or slow FPS.
    ///
    /// @see add_event
    ///
    void add_event_next_frame(std::function<void ()> func);

    ///
    /// Add an event with a time duration to run for. e.g. a timer
    /// This is wrapped in a normal event (like one passed to
    /// add_event) internally and internally managed.
    ///
    /// @param duration The time duration to run the event for
    /// @param func a boolean return and float argument
    /// lambda. Return true to keep it in the queue and return false
    /// to remove it. The float is intended to be a percentage of
    /// completion. 0.0 is 0% and 1.0 is 100%. This is calculated
    /// based on the duration, the current time and the time the event was added to the
    /// queue
    void add_timed_event(GameTime::duration duration, std::function<bool (float)> func);

    ///
    /// Processes all events in the current frame queue
    ///
    void process_events(InterpreterContext &interpreter_context);

};

#endif

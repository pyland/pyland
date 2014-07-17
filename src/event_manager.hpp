#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <mutex>
#include <thread>

///
/// The event manager class
/// This class needs to take events from Python thread and the engine
/// and put them into a priority queue. The methods to put the events
/// into the queue and to get the events from the queue are
/// thread-safe provided the Event object is immutable.
///
class EventManager {
///
/// The mutex to control access to the queueb
///
std::mutex queue_mutex;


///
///The priority queue for events
///
    //std::queue< queue;

public:
EventManager();
~EventManager();
void register_event();


};

#endif

#ifndef ENTITYTHREAD_H
#define ENTITYTHREAD_H

#include <future>
#include <thread>
#include "api.hpp"

class Interpreter;

class EntityThread {
    public:
        ///
        /// Construct a EntityThread from a Entity object.
        ///
        /// Spawns a new thread.
        ///
        EntityThread(Interpreter *interpreter, Entity &entity);
        ~EntityThread();

        ///
        /// Get the id of the thread according to CPython.
        ///
        long get_thread_id();

        ///
        /// Try to nicely kill the thread; avoiding corruption where possible.
        ///
        /// Neither termination nor safety is guaranteed.
        ///
        void halt_soft();

        ///
        /// Try to kill the thread without mortal concerns for such things as life and death.
        ///
        /// Neither termination nor safety is guaranteed, although
        /// termination is more likely than with halt_soft.
        ///
        void halt_hard();

        ///
        /// Check if an API call has been made since last call to clean.
        ///
        /// @return Whether API call has been made since last call to clean.
        ///
        bool is_dirty();

        ///
        /// Remember state when last called, for use with is_dirty.
        ///
        /// @see is_dirty
        ///
        void clean();

    private:
        /// 
        /// Reference to API's wrapped Entity.
        ///
        /// API calls are passed through to this object.
        /// 
        Entity &entity;

        ///
        /// Thread spawned by this EntityThread
        ///
        std::unique_ptr<std::thread> thread;

        ///
        /// Stores the call count from last time clean was called.
        ///
        /// Largest definitely-available integer,
        /// gives contingency because a large number
        /// of calls to APIs could occur.
        ///
        uint64_t previous_call_number;

        /// 
        /// Python's nonstandard interpretation of the thread's id.
        ///
        /// Used to send asynchronous exceptions to threads. 
        /// 
        long thread_id;
        std::future<long> thread_id_future;

        ///
        ///
        ///
        std::shared_ptr<boost::python::api::object> entity_object;

        ///
        ///
        ///
        void finish();
};

#endif

#ifndef ENTITYTHREAD_H
#define ENTITYTHREAD_H

#include <future>
#include <map>
#include <thread>
#include "api.hpp"
#include "interpreter_context.hpp"
#include "locks.hpp"

class Interpreter;
class EntityThread;

using EntityThreads        = lock::Lockable<std::vector<std::weak_ptr<EntityThread>>>;
using LockableEntityThread = lock::Lockable<std::shared_ptr<EntityThread>>;

///
/// Container that abstracts the daemon threads for
/// the game's entities.
///
/// @warning
///     This needs to be, but is not, thread safe in many places.
///
class EntityThread {
    private:
        // TODO: Comment
        PyObject *make_base_async_exception(PyObject *base, const char *name);

        /// 
        /// Reference to API's wrapped Entity.
        ///
        /// API calls are passed through to this object.
        /// 
        Entity &entity;

        ///
        /// Thread spawned by this EntityThread.
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
        /// The interpreter context to lock on.
        ///
        /// This is needed as an attribute as destruction requires
        /// locking in order to safely destroy Python objects.
        ///
        InterpreterContext interpreter_context;

        /// 
        /// Python's nonstandard interpretation of the thread's ID.
        /// Might not be set at any point, so usage of get_thread_id
        /// is recommended even for internal usage.
        ///
        /// Used to send asynchronous exceptions to threads. 
        /// 
        long thread_id;

        ///
        /// A private future used to get the thread's ID asynchronously.
        ///
        /// The spawned thread has the ability to set the value of this
        /// future. This future is only valid once and should be handled
        /// only through get_thread_id.
        ///
        std::future<long> thread_id_future;

        ///
        /// An Entity wrapped in a Python object.
        ///
        /// This exists to be neatly cleaned up in the destructor.
        ///
        std::shared_ptr<boost::python::api::object> entity_object;

        ///
        /// Finish and join the spawned thread.
        ///
        void finish();

    public:
        // TODO: Comment
        enum class Signal {
            RESTART = 100,
            STOP    = 200,
            KILL    = 300
        };

        ///
        /// Construct a EntityThread from a Entity object.
        ///
        /// Spawns a new thread.
        ///
        /// @param interpreter_context
        ///     An interpreter context to lock the GIL on.
        ///     The GIL is locked on the main thread.
        ///
        /// @param entity
        ///     The entity to construct the daemon for.
        ///
        EntityThread(InterpreterContext interpreter_context, Entity &entity);

        ///
        /// Close the thread and shut down neatly.
        ///
        /// If the thread is not finished, it is killed. This does
        /// not return until the thread is joined.
        ///
        ~EntityThread();

        ///
        /// Get the ID of the thread according to CPython.
        ///
        /// @warning
        ///     Not thread safe.
        ///
        /// @return
        ///     The ID of the thread according to CPython.
        ///
        long get_thread_id();

        ///
        /// Try to nicely kill the thread; avoiding corruption where possible.
        ///
        /// @warning
        ///     This needs to be, but is not, totally thread safe.
        ///
        /// Neither termination nor safety isguaranteed.
        ///
        /// TODO: Document
        ///
        void halt_soft(Signal signal);

        ///
        /// Try to kill the thread without mortal concerns for such things as life and death.
        ///
        /// @warning
        ///     This needs to be, but is not, totally thread safe.
        ///
        /// Neither termination nor safety is guaranteed, although
        /// termination is more likely than with halt_soft.
        ///
        void halt_hard();

        ///
        /// Check if an API call has been made since last call to clean.
        ///
        /// @warning
        ///     Only supported for usage from the kill thread.
        ///
        /// @return
        ///     Whether an API call has been made since last call to clean.
        ///
        /// @see clean
        ///
        bool is_dirty();

        ///
        /// Remember state when last called, for use with is_dirty.
        ///
        /// @warning
        ///     Only supported for usage from the kill thread.
        ///
        /// @see is_dirty
        ///
        void clean();

        // TODO: Comment
        PyObject *Py_BaseAsyncException;
        // TODO: Comment
        std::map<Signal, PyObject *> signal_to_exception;
};

#endif

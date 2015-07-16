#ifndef ENTITYTHREAD_H
#define ENTITYTHREAD_H

#include "python_embed_headers.hpp"

#include <atomic>
#include <future>
#include <map>
#include <thread>
#include <list>
#include "dispatcher.hpp"
#include "interpreter_context.hpp"
#include "locks.hpp"
#include "entity.hpp"

class Interpreter;
//class Entity;
class EntityThread;

using EntityThreads = lock::Lockable<std::vector<std::weak_ptr<EntityThread>>>;

///
/// A lock::Lockable<std::shared_ptr<EntityThread>>
/// with additional public event dispatchers.
///
class LockableEntityThread : public lock::Lockable<std::shared_ptr<EntityThread>> {
    public:
        //using lock::Lockable<std::shared_ptr<EntityThread>>::Lockable;
        LockableEntityThread();
        LockableEntityThread(std::shared_ptr<EntityThread> value);
        LockableEntityThread(std::shared_ptr<EntityThread> value, std::shared_ptr<std::mutex> lock);

        // TODO: Rearchitechture
        Dispatcher<> event_run;
        // TODO: Rearchitechture
        Dispatcher<> event_restart;
        // TODO: Rearchitechture
        Dispatcher<> event_stop;
        // TODO: Rearchitechture
        Dispatcher<> event_kill;
};

///
/// Container that abstracts the daemon threads for
/// the game's entities.
///
/// @warning
///     This needs to be, but is not, thread safe in many places.
///
class EntityThread {
    private:
        ///
        /// Private shortcut to making exceptions which can
        /// be thrown inside the Python-running thread in order
        /// to control it.
        ///
        /// The PyObject is created with a single reference count
        /// and this must be manually decreased before the object
        /// is thrown away.
        ///
        /// @param base
        ///     The base class, a BaseException subclass, to subclass.
        ///
        /// @param name
        ///     The name to give the new object.
        ///
        PyObject *make_base_async_exception(PyObject *base, const char *name);

        ///
        /// Reference to API's wrapped Entity.
        ///
        /// API calls are passed through to this object.
        ///
        //Entity &entity;
    
        //TODO: Make this not so hacky!
        std::list<Entity> entities;

        ///
        /// Thread spawned by this EntityThread.
        ///
        std::thread thread;

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
        /// A private future used to get the thread's state asynchronously.
        ///
        /// The spawned thread will set this value when exiting (even by exception).
        /// This is used to ensure destruction.
        ///
        std::atomic<bool> thread_finished;

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
        ///
        /// Names for all of the exception types that can send signals
        /// to the running Python threads.
        ///
        /// RESTART: Stop and then start the process, whether or not it's already running.
        ///
        /// STOP: Stop the process and keep it in standby.
        ///
        /// KILL: Finish usage of the process, and terminate permanantly.
        ///
        enum class Signal {
            RESTART,
            STOP,
            KILL
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
        //EntityThread(InterpreterContext interpreter_context, Entity &entity);

        //TODO: Make this not hacked together
        EntityThread(InterpreterContext interpreter_context, std::list<Entity> entities);

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
        /// Try to nicely kill the thread, or send another signal,
        /// avoiding corruption where possible.
        ///
        /// @warning
        ///     This needs to be, but is not, totally thread safe.
        ///
        /// Neither termination nor safety isguaranteed.
        ///
        /// @param signal
        ///     The operation to perform.
        ///
        /// @see Signal
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

        ///
        /// The base exception all Signal exceptions derive from.
        ///
        PyObject *Py_BaseAsyncException;

        ///
        /// A map of all signals (not Py_BaseAsyncException) that can
        /// be sent to threads, and their corresponding exception.
        ///
        std::map<Signal, PyObject *> signal_to_exception;
};

#endif

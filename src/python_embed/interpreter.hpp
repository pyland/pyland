#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "Python.h"
#include "python_embed_headers.hpp"

#include <atomic>
#include <boost/filesystem.hpp>
#include <boost/python.hpp>
#include <memory>
#include <vector>
#include "entitythread.hpp"
#include "interpreter_context.hpp"
#include "thread_killer.hpp"
#include "locks.hpp"

///
/// The Python Interpreter singleton.
/// Multiple initializations will throw errors.
///
/// Entities are registered with this class in order to
/// give them daemons.
///
class Interpreter {
    public:
        ///
        /// Interpreter contructor. Throws if called more than once.
        ///
        /// Initializes CPython.
        ///
        /// @param function_wrappers
        ///     The file that wraps the C++ classes for CPython's usage.
        ///
        Interpreter(boost::filesystem::path function_wrappers);

        ///
        /// Deconstruct interpreter.
        /// Destroys all child threads and Python objects,
        /// safely when it can.
        ///
        /// @warning
        ///     This does not uninitialize CPython, as it is
        ///     unclear whether it is compatible with boost::python.
        ///
        ~Interpreter();

        ///
        /// Give an entity to the interpreter, which will be wrapped
        /// by the Python-side API. This is the canonical way of giving
        /// a daemon to an entity.
        ///
        /// Returns the thread in a lockable object. This can be used
        /// to tell the script to perform actions like starting and stopping.
        ///
        /// When the thread is discarded, it will be destroyed. This is a
        /// blocking operation.
        ///
        /// @deprecated
        ///     Will be changed to accept an ID.
        ///
        /// @param entity
        ///     The game entity to wrap.
        ///
        /// @return
        ///     The thread in a lockable object. This can be used to tell
        ///     the script to perform actions like starting and stopping.
        ///
        ///     When the thread is discarded, it will be destroyed. This is
        ///     a blocking operation. 
        ///
        LockableEntityThread register_entity(Entity &entity);

        ///
        /// The main thread of the spawned interpreter.
        ///
        /// Currently public to allow spawning new threads,
        /// although this will later be made a more private
        /// feature.
        ///
        /// @deprecated
        ///
        //PyThreadState *main_thread_state;

        ///
        /// An interpreter context used to lock on the current thread
        /// and to pass to other functions.
        ///
        /// Primarily used in register_entity.
        ///
        /// @see register_entity
        ///
        /// @warning
        ///     Potentially only temporarily public.
        ///
        InterpreterContext interpreter_context;

    private:
        ///
        /// Cannot copy interpreters for obvious reasons.
        ///
        Interpreter(const Interpreter&) = delete;

        ///
        /// A flag to ensure that this is a singleton.
        /// Set when an interpreter has been initialized.
        ///
        /// Destruction does not reset flag. This may change
        /// between releases.
        ///
        /// Will be removed if support is added for multiple
        /// interpreters.
        ///
        static std::atomic_flag initialized;

        ///
        /// The thread that kills EntityThreads when they
        /// have not called a wrapped API sufficiently recently.
        ///
        std::unique_ptr<ThreadKiller> thread_killer;

        ///
        /// The spawed threads, mainly for usage by kill_thread.
        ///
        /// Ownership is kept to ensure the lifetime is greater than
        /// the killer thread's.
        ///
        /// @see thread_killer
        ///
        EntityThreads entitythreads;

        ///
        /// Internal API to start CPython. Called on creation.
        ///
        PyThreadState *initialize_python();

        ///
        /// Internal API to finish with CPython. Called on destruction.
        ///
        void deinitialize_python();
};

#endif

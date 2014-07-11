#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <atomic>
#include <boost/filesystem.hpp>
#include <boost/python.hpp>
#include "thread_killer.hpp"

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
        /// Convenience function to import a file from a boost::filesystem::path.
        ///
        /// This requires an interpreter instance to be running (or at least
        /// CPython to be started) but does not need a reference to it.
        ///
        /// @param filename
        ///     The path to import. This may be relative, but it is not
        ///     well defined what path it is relative to, and it is permitted
        ///     not to give working behaviour in that case.
        ///
        ///     The filename must end in .py or .so.
        ///
        ///         - If it is .py, it is assumed to be a valid Python file.
        ///
        ///         - If it is .so, it is assumed to be a valid Python-importable
        ///           shared object file. The name to import under is the stem
        ///           of the path, so these must match.
        ///
        ///         - If it is neither, an std::runtime_error is thrown.
        ///
        /// @return
        ///     A Python module object.
        ///
        static boost::python::api::object import_file(boost::filesystem::path filename);

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
        /// @deprecated
        ///     Will be changed to accept an ID.
        ///
        /// @param entity
        ///     The game entity to wrap.
        ///
        void register_entity(Entity entity);

        ///
        /// The main thread of the spawned interpreter.
        ///
        /// Currently public to allow spawning new threads,
        /// although this will later be made a more private
        /// feature.
        ///
        /// @deprecated
        ///
        PyThreadState *main_thread_state;

    private:
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
        lock::Lockable<std::vector<std::unique_ptr<EntityThread>>> entitythreads;

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

#ifndef INTERPRETER_CONTEXT_H
#define INTERPRETER_CONTEXT_H

#include "python_embed_headers.hpp"

#include <boost/filesystem.hpp>
#include <boost/python.hpp>

///
/// A simple wrapper class for PyThreadState,
/// used to represent the main thread of an
/// interpreter, and its interpreter state.
///
class InterpreterContext {
    public:
        ///
        /// Initialize the InterpreterContext.
        ///
        /// @param main_thread_state
        ///     The thread state to spawn.
        ///
        InterpreterContext(PyThreadState *main_thread_state);

        ///
        /// TODO
        ///
        PyThreadState *get_threadstate();

        ///
        /// TODO
        ///
        PyInterpreterState *get_interpreterstate();

        ///
        /// Convenience function to import a file from a boost::filesystem::path.
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
        boost::python::api::object import_file(boost::filesystem::path filename);

    private:
        ///
        /// TODO
        ///
        PyThreadState *main_thread_state;
};

#endif
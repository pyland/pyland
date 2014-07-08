#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <boost/filesystem.hpp>
#include <boost/python.hpp>
#include <mutex>
#include "thread_killer.h"


class Interpreter {
    public:
        static boost::python::api::object import_file(boost::filesystem::path filename);

        Interpreter(boost::filesystem::path function_wrappers);
        ~Interpreter();
        void register_entity(Entity entity);
        PyThreadState *main_thread_state;

    private:
        static std::mutex initialized;
        lock::Lockable<std::vector<std::unique_ptr<EntityThread>>> entitythreads;
        std::unique_ptr<ThreadKiller> thread_killer;

        PyThreadState *initialize_python();
        void deinitialize_python();
};

#endif

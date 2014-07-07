#include <boost/filesystem.hpp>
#include <mutex>
#include "thread_killer.h"

class Interpreter {
    public:
        Interpreter(boost::filesystem::path working_directory,
                    boost::filesystem::path function_wrappers);

        ~Interpreter();

        void register(Entity entity);

    private:
        static std::mutex initialized;
        Lockable<std::vector<PlayerThread>> playerthreads;
        std::unique_ptr<ThreadKiller> thread_killer;

        PyInterpreterState *Interpreter::initialize_python();
        py::api::object Interpreter::import_file(boost::filesystem::path filename);
        void Interpreter::deinitialize_python();
};

#include <algorithm>
#include <atomic>
#include <boost/filesystem.hpp>
#include <boost/python.hpp>
#include <map>
#include <mutex>
#include "api.h"
#include "entitythread.h"
#include "interpreter.h"
#include "locks.h"
#include "make_unique.h"
#include "print_debug.h"
#include "thread_killer.h"

std::atomic_flag Interpreter::initialized = ATOMIC_FLAG_INIT;

Interpreter::Interpreter(boost::filesystem::path function_wrappers) {

    if (initialized.test_and_set()) {
        throw std::runtime_error("Interpreter initialized twice where only a single initialization supported");
    }

    print_debug << "Interpreter: Started" << std::endl;

    main_thread_state = initialize_python();

    thread_killer = std::make_unique<ThreadKiller>(entitythreads);
    print_debug << "Interpreter: Spawned Kill thread" << std::endl;

    // All Python errors should result in a Python traceback    
    try {
        // TODO: decide whether to set sys.path

        // Import to allow conversion of classes, but no need to keep module reference
        import_file(function_wrappers);
    }
    catch (py::error_already_set) {
        PyErr_Print();
    }

    // Release GIL; thread_killer can start killing now
    // and EntityThreads can be created without deadlocks
    PyEval_ReleaseLock();

    print_debug << "Interpreter: Released GIL " << std::endl;
    print_debug << "Interpreter: Interpreter created " << std::endl;
}

PyThreadState *Interpreter::initialize_python() {
    Py_Initialize();
    PyEval_InitThreads();

    print_debug << "Interpreter: Initialized Python" << std::endl;

    return PyThreadState_Get();
}

void Interpreter::register_entity(Entity entity) {
    std::lock_guard<std::mutex> lock(entitythreads.lock);

    entitythreads.value.push_back(std::move(std::make_unique<EntityThread>(this, entity)));
}

Interpreter::~Interpreter() {
    thread_killer->finish();
    print_debug << "Interpreter: Finished kill thread" << std::endl;

    {
        // Lock not acutally needed; thread_killer is dead
        // However, this keeps the guarantees (locked while edited) safe,
        // so is good practice
        std::lock_guard<std::mutex> lock(entitythreads.lock);
        entitythreads.value.clear();
    }

    deinitialize_python();
}

void Interpreter::deinitialize_python() {
    PyEval_RestoreThread(main_thread_state);
    print_debug << "Interpreter: Deinitialized Python" << std::endl;
}





std::map<std::string, std::string> extension_to_importer = {
    {".py", "SourceFileLoader"},
    {".so", "ExtensionFileLoader"}
};

py::api::object Interpreter::import_file(boost::filesystem::path filename) {    
    std::string extension = filename.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    auto importer = extension_to_importer.at(extension);

    print_debug << "Interpreter: Importing " << filename << " with " << importer << " (for " << extension << " files)." << std::endl;
    auto importlib_machinery_module = py::import("importlib.machinery");

    print_debug << "Interpreter: Imported loader module" << std::endl;

    // Get and initialize loader
    auto FileLoader_object = importlib_machinery_module.attr(importer.c_str());
    auto loader_object = FileLoader_object(filename.stem().string(), filename.string());

    print_debug << "Interpreter: Got loader" << std::endl;

    return loader_object.attr("load_module")();
}

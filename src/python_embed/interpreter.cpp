#include <algorithm>
#include <atomic>
#include <boost/filesystem.hpp>
#include <boost/python.hpp>
#include <map>
#include <mutex>
#include <string>
#include "api.hpp"
#include "entitythread.hpp"
#include "interpreter.hpp"
#include "locks.hpp"
#include "make_unique.hpp"
#include "print_debug.hpp"
#include "thread_killer.hpp"

// Initialize unset.
// WARNING: This is the only valid way to initialize this type.
std::atomic_flag Interpreter::initialized = ATOMIC_FLAG_INIT;

Interpreter::Interpreter(boost::filesystem::path function_wrappers) {
    // Make sure it's a singleton
    if (initialized.test_and_set()) {
        throw std::runtime_error("Interpreter initialized twice where only a single initialization supported");
    }

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
        throw std::runtime_error("Cannot spawn thread killer. Bailing early.");
    }

    // Release GIL; thread_killer can start killing now
    // and EntityThreads can be created without deadlocks
    PyEval_ReleaseLock();
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

    // Create thread and move to vector.
    auto new_entity = std::make_unique<EntityThread>(this, entity);
    entitythreads.value.push_back(std::move(new_entity));
}

Interpreter::~Interpreter() {
    // Join killer
    thread_killer->finish();
    print_debug << "Interpreter: Finished kill thread" << std::endl;

    // Exorcise daemons
    {
        // Lock not acutally needed; thread_killer is dead
        // However, this keeps the guarantees (locked while edited) safe,
        // so is good practice
        std::lock_guard<std::mutex> lock(entitythreads.lock);
        entitythreads.value.clear();
    }

    // Finished Python
    deinitialize_python();
}

void Interpreter::deinitialize_python() {
    PyEval_RestoreThread(main_thread_state);
    print_debug << "Interpreter: Deinitialized Python" << std::endl;
}

// Only supported extensions
std::map<std::string, std::string> extension_to_importer = {
    {".py", "SourceFileLoader"},
    {".so", "ExtensionFileLoader"}
};

py::api::object Interpreter::import_file(boost::filesystem::path filename) {    
    // Get lowercase extension
    std::string extension = filename.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    // Get importer for this filetype, and initialize loader
    auto importer = extension_to_importer.at(extension).c_str();
    auto FileLoader_object = py::import("importlib.machinery").attr(importer);
    auto loader_object = FileLoader_object(filename.stem().string(), filename.string());

    // Load the module
    return loader_object.attr("load_module")();
}

#include <boost/filesystem.hpp>
#include <boost/python.hpp>
#include <map>
#include "interpreter_context.hpp"

namespace py = boost::python;

InterpreterContext::InterpreterContext(PyThreadState *main_thread_state):
    main_thread_state(main_thread_state) {}

PyThreadState *InterpreterContext::get_threadstate() {
    return main_thread_state;
}

PyInterpreterState *InterpreterContext::get_interpreterstate() {
    return main_thread_state->interp;
}


// Only supported extensions
std::map<std::string, std::string> extension_to_importer = {
    {".py", "SourceFileLoader"},
    {".so", "ExtensionFileLoader"}
};

py::api::object InterpreterContext::import_file(boost::filesystem::path filename) {    
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

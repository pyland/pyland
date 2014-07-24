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


py::api::object InterpreterContext::import_file(boost::filesystem::path filename) {    

    std::string name = filename.stem().string();

    py::list paths;
    paths.append(filename.parent_path().string());

    auto imp_module = py::import("imp");
    auto module_data = imp_module.attr("find_module")(name, paths);
    return imp_module.attr("load_module")(
        name,
        py::api::object(module_data[0]), // file
        py::api::object(module_data[1]), // pathname
        py::api::object(module_data[2])  // description
    );
}

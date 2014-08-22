#include "python_embed_headers.hpp"

#include <string>
#include <boost/python.hpp>
#include <iostream>
#include "api.hpp"

namespace py = boost::python;

BOOST_PYTHON_MODULE(wrapper_functions) {
    py::class_<Entity, boost::noncopyable>("Entity", py::no_init)
        .def_readwrite("name",   &Entity::name)
        .def_readwrite("id",     &Entity::id)
        .def("move",             &Entity::move)
        .def("walkable",         &Entity::walkable)
        .def("monologue",        &Entity::monologue)
        .def("cut",              &Entity::cut)
        .def("look",             &Entity::look)
        .def("__set_game_speed", &Entity::__set_game_speed)
        .def("print_debug",      &Entity::py_print_debug)
        .def("print_dialogue",   &Entity::py_print_dialogue)
        .def("update_status",    &Entity::py_update_status);
}

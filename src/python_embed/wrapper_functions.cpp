#include "python_embed_headers.hpp"

#include <string>
#include <boost/python.hpp>
#include <iostream>
#include "api.hpp"

namespace py = boost::python;

BOOST_PYTHON_MODULE(wrapper_functions) {
    py::class_<Entity, boost::noncopyable>("Entity", py::no_init)
        .def_readwrite("id",      &Entity::id)
        .def_readwrite("name",    &Entity::name)
        .def("__set_game_speed",  &Entity::__set_game_speed)
        .def("cut",               &Entity::cut)
        .def("get_instructions",  &Entity::get_instructions)
        .def("get_retrace_steps", &Entity::get_retrace_steps)
        .def("look",              &Entity::look)
        .def("monologue",         &Entity::monologue)
        .def("move",              &Entity::move)
        .def("print_debug",       &Entity::py_print_debug)
        .def("print_dialogue",    &Entity::py_print_dialogue)
        .def("read_message",      &Entity::read_message)
        .def("update_status",     &Entity::py_update_status)
        .def("walkable",          &Entity::walkable)
        .def("get_x_position",    &Entity::get_x_position)
        .def("get_y_position",    &Entity::get_y_position);
}

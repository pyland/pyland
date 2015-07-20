#include "python_embed_headers.hpp"

#include <string>
#include <boost/python.hpp>
#include <iostream>
#include "entity.hpp"

namespace py = boost::python;

BOOST_PYTHON_MODULE(wrapper_functions) {
    py::class_<Entity, boost::noncopyable>("Entity", py::no_init)
        .def_readwrite("id",      &Entity::id)
        .def_readwrite("name",    &Entity::name)
        .def("__set_game_speed",  &Entity::__set_game_speed)
        .def("get_instructions",  &Entity::get_instructions)
        .def("get_retrace_steps", &Entity::get_retrace_steps)
        .def("monologue",         &Entity::monologue)
        .def("move_east",         &Entity::move_east)
        .def("move_west",         &Entity::move_west)
        .def("move_north",        &Entity::move_north)
        .def("move_south",        &Entity::move_south)
        .def("print_debug",       &Entity::py_print_debug)
        .def("print_dialogue",    &Entity::py_print_dialogue)
        .def("read_message",      &Entity::read_message)
        .def("update_status",     &Entity::py_update_status)
        .def("walkable",          &Entity::walkable)
        .def("callback_test",     &Entity::callback_test)
        .def("get_name",          &Entity::get_name)
        .def("get_location",      &Entity::get_location)
        .def("focus",             &Entity::focus);
}

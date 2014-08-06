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
        .def("__set_game_speed", &Entity::__set_game_speed)
        .def("print_debug",      &Entity::py_print_debug)
        .def("print_dialogue",   &Entity::py_print_dialogue)
        .def("update_status",    &Entity::py_update_status);

    py::class_<Vec2D>("Vec2D", py::init<int, int>())
        .def(py::self + py::other<Vec2D>())
        .def_readwrite("x", &Vec2D::x)
        .def_readwrite("y", &Vec2D::y);
}

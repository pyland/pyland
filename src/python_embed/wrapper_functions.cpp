#include <string>
#include <boost/python.hpp>
#include <iostream>
#include "api.hpp"

namespace py = boost::python;

BOOST_PYTHON_MODULE(wrapper_functions) {
    py::class_<Entity, boost::noncopyable>("Entity", py::no_init)
        .def("move",        &Entity::move)
        .def("walkable",    &Entity::walkable)
        .def("monologue",   &Entity::monologue)
        .def("run_script",  &Entity::run_script)
        .def("give_script", &Entity::give_script);

    py::class_<Vec2D>("Vec2D", py::init<int, int>())
        .def(py::self + py::other<Vec2D>())
        .def_readwrite("x", &Vec2D::x)
        .def_readwrite("y", &Vec2D::y);
}

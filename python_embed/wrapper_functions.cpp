#include <string>
#include <boost/python.hpp>
#include <iostream>
#include "api.h"

namespace py = boost::python;

BOOST_PYTHON_MODULE(wrapper_functions) {
    py::class_<Player, boost::noncopyable>("Player", py::no_init)
        .def("move",        &Player::move)
        .def("monologue",   &Player::monologue)
        .def("run_script",  &Player::run_script)
        .def("give_script", &Player::give_script)
        .def_readwrite("in_call", &Player::in_call);

    py::class_<Vec2D>("Vec2D", py::init<int, int>())
        .def(py::self + py::other<Vec2D>())
        .def_readwrite("x", &Vec2D::x)
        .def_readwrite("y", &Vec2D::y);
}

#include <string>
#include <boost/python.hpp>
#include <iostream>
#include "api.h"

namespace py = boost::python;

BOOST_PYTHON_MODULE(wrapper_functions) {
	py::class_<Player>("Player", py::init<int, int>())
		.def("move",           &Player::move)
		.def("speak_position", &Player::speak_position);
}

#include <string>
#include <boost/python.hpp>
#include <iostream>
#include "api.h"

namespace py = boost::python;

BOOST_PYTHON_MODULE(wrapper_functions) {
	py::class_<Player>("Player", py::no_init)
		.def("move",        &Player::move)
		.def("monologue",   &Player::monologue)
		.def("run_script",  &Player::run_script)
		.def("give_script", &Player::give_script);
}

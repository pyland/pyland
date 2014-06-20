#include <string>
#include <iostream>
#include <boost/python.hpp>
#include <boost/filesystem.hpp>
#include "api.h"

namespace py = boost::python;

int main(int argc, char **argv) {
	Py_Initialize();

	// Get the current directory and add it to Python's sys.path
	boost::filesystem::path workingDir = boost::filesystem::absolute("./").normalize();
	py::api::object sys_paths = py::import("sys").attr("path");
	sys_paths.attr("append")(py::str(workingDir.string()));

	try {
		py::api::object main_module = py::import("__main__");
		py::api::object main_namespace = main_module.attr("__dict__");

		py::api::object wrapper_module = py::import("wrapper_functions");
		main_module.attr("wrapper_functions") = wrapper_module;

		Player my_player = Player(Vec2D(90, 102), Direction::UP, "John");
		main_module.attr("preplayer") = py::object(my_player);

		py::exec(
			"player = wrapper_functions.Player(2, 6)\n"
			"player.speak_position()\n"
			"player.move(10, -3)\n"
			"player.speak_position()\n"
			"preplayer.speak_position()\n",
			main_namespace
		);
	}
	catch (boost::python::error_already_set) {
		PyErr_Print();
	}
}

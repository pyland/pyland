#include <boost/python.hpp>
#include <boost/regex.hpp>
#include <fstream>
#include <sstream>
#include <string>

#include "api.hpp"
#include "engine_api.hpp"
#include "print_debug.hpp"

namespace py = boost::python;

Vec2D::Vec2D(int x, int y): x(x), y(y) {};

Vec2D Vec2D::operator+(Vec2D other) {
    return Vec2D(x + other.x, y + other.y);
}

Vec2D Vec2D::operator-(Vec2D other) {
    return Vec2D(x - other.x, y - other.y);
}

void Vec2D::operator+=(Vec2D other) {
    x += other.x;
    y += other.y;
}

void Vec2D::operator-=(Vec2D other) {
    x -= other.x;
    y -= other.y;
}

std::ostream& operator<<(std::ostream& out, Vec2D in) {
    return out << in.to_string();
}

std::string Vec2D::to_string() {
    std::ostringstream stringStream;
    stringStream << "(" << x << " " << y << ")";
    return stringStream.str();
}



Entity::Entity(Vec2D start, std::string name, int id):
    start(start), position(start), script(""), id(id), call_number(0) {
        this->name = std::string(name);
        Engine::move_object(id, start.x, start.y);
}

bool Entity::move(int x, int y) {
    ++call_number;

    if (Engine::move_object(id, x, y)) {
        position += Vec2D(x, y);
        return true;
    };

    return false;
}

bool Entity::walkable(int x, int y) {
    ++call_number;
    auto new_position = position + Vec2D(x, y);
    return Engine::walkable(new_position.x, new_position.y);
}

void Entity::monologue() {
    std::cout << "I am " << name << " and I am standing at " << position << "!" << std::endl;
}

// WARNING: DEPRECATED
void Entity::run_script() {
    ++call_number;
    script(py::ptr(this));
}


//  █████   ██████  ██████  ██████  ██████   █████   ████   ██████  ██████  █████
//  ██  ██  ██      ██  ██  ██  ██  ██      ██      ██  ██    ██    ██      ██  ██
//  ██  ██  ████    ██████  ██████  ████    ██      ██████    ██    ████    ██  ██
//  ██  ██  ██      ██      ██ ██   ██      ██      ██  ██    ██    ██      ██  ██
//  █████   ██████  ██      ██  ██  ██████   █████  ██  ██    ██    ██████  █████

// WARNING: DEPRECATED
void Entity::give_script(py::api::object main_namespace) {
    py::api::object tempoary_scope = main_namespace.attr("copy")();

    // read users py from file
    std::string user_py_unparsed = read_file("python_embed/" + name + ".py");
    // fix indenting
    boost::regex replace("\\n");
    std::string user_py = boost::regex_replace(user_py_unparsed, replace, "\n    ");
    // wrap with our code
    std::string wrapped_py = read_file("python_embed/py_wrapper.py")+ "\n    " + user_py;
    print_debug << wrapped_py << std::endl;
    // evaluate code & extract script function
    script = py::exec(wrapped_py.c_str(), tempoary_scope);
    script = tempoary_scope["script"];
   // py::import("dis").attr("dis")(script);
}

// WARNING: DEPRECATED
std::string Entity::read_file(std::string loc) {
    //open the input file
    std::ifstream in_file(loc);

    if (in_file.is_open()) { 
        std::stringstream stringstream;

        //read the file
        stringstream << in_file.rdbuf();
        return stringstream.str();
    }
    else {
        print_debug << "file opening unsuccessful" << std::endl;
        return "";
    }
}

void Entity::py_print_debug(std::string text) {
    print_debug << text << std::endl;
}

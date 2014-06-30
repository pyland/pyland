#include <boost/python.hpp>
#include <boost/regex.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "api.h"
#include "debug.h"

namespace py = boost::python;

Vec2D::Vec2D(int x, int y): x(x), y(y) {};

Vec2D Vec2D::operator+(Vec2D other) {
    return Vec2D(x + other.x, y + other.y);
}

void Vec2D::operator+=(Vec2D other) {
    x += other.x;
    y += other.y;
}

std::ostream& operator<<(std::ostream& out, Vec2D in) {
    return out << in.to_string();
}

std::string Vec2D::to_string() {
    std::ostringstream stringStream;
    stringStream << "(" << x << " " << y << ")";
    return stringStream.str();
}



Player::Player(Vec2D start, std::string name):
    position(start), script("") {
        this->name = std::string(name);
}

uint64_t Player::call_number = 0;

void Player::move(Vec2D by) {
    position += by;
}

void Player::monologue() {
    std::cout << "I am " << name << " and I am standing at " << position << "!" << std::endl;
}

void Player::run_script() {
    ++call_number;
    script(py::ptr(this));
}


void Player::give_script(py::api::object main_namespace) {
    py::api::object tempoary_scope = main_namespace.attr("copy")();
    std::string from_file =
    // TODO: find a more scalable approach
    "def move(x):\n\t"
    "player.move(x)\n"
    "def monologue(x):\n\t"
    "player.monologue(x)\n"
    "north, south, east, west = Vec2D(0, 1), Vec2D(0, -1), Vec2D(1, 0), Vec2D(-1, 0)\n" 
    "def script(player):\n\t" + read_file() + "\n\treturn script\n";
    print_debug << from_file << std::endl;
    script = py::exec(from_file.c_str(), tempoary_scope);
    script = tempoary_scope["script"];
   // py::import("dis").attr("dis")(script);
}

std::string Player::read_file() {
    std::string loc = name + ".py";
    std::ifstream inFile (loc); //open the input file
    if (inFile.is_open()) { 
        std::stringstream strStream;
        strStream << inFile.rdbuf(); //read the file
        std::string old_text = strStream.str();
        boost::regex replace("\\n");
        std::string new_text = boost::regex_replace(old_text, replace, "\n\t");
        return new_text;
    } else {
        print_debug << "file opening unsuccessful" << std::endl;
        return "";
    }
}

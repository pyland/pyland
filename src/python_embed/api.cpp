#include "python_embed_headers.hpp"

#include <boost/python.hpp>
#include <boost/regex.hpp>
#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <sstream>
#include <string>

#include "api.hpp"
#include "engine_api.hpp"

namespace py = boost::python;

Vec2D::Vec2D(int x, int y): x(x), y(y) {}

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
    start(start), script(""), id(id), call_number(0) {
        this->name = std::string(name);
}

bool Entity::move(int x, int y) {
    ++call_number;

    if (Engine::move_object(id, Vec2D(x, y))) {
        return true;
    };

    return false;
}

bool Entity::walkable(int x, int y) {
    ++call_number;
    return Engine::walkable(Engine::find_object(id) + Vec2D(x, y));
}

void Entity::monologue() {
    // TODO: Hook up to proper speaking.
    std::cout << "I am " << name << " and I am standing at " << Engine::find_object(id) << "!" << std::endl;
}


void Entity::py_print_debug(std::string text) {
    LOG(INFO) << text;
}

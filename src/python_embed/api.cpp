#include <boost/python.hpp>
#include <boost/regex.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include "api.h"
#include "main.hpp"
#include "print_debug.h"

#define WRAPPING_ENABLED true
#define TILESIZE_PIXELS 32

enum class TileType {
    WALKABLE,
    UNWALKABLE,
    KILLER
};

std::map<int, TileType> tile_to_type({
    { 8, TileType::WALKABLE},   // Board
    {12, TileType::WALKABLE},   // Flowers
    {64, TileType::WALKABLE},   // Grass

    { 2, TileType::UNWALKABLE}, // Edged wall
    {13, TileType::UNWALKABLE}, // Water
    {14, TileType::UNWALKABLE}, // Wall
    {21, TileType::UNWALKABLE}, // Hideous ice

    {57, TileType::KILLER},     // Trapdoor (set)
    {74, TileType::KILLER}      // Lava
});

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
    start(start), position(start), script(""), id(id) {
        this->name = std::string(name);
        move_object(id, TILESIZE_PIXELS * float(start.x), TILESIZE_PIXELS * float(start.y));
}

uint64_t Entity::call_number = 0;

bool Entity::move(int x, int y) {
    ++call_number;

    auto cached_position = position;
    position += Vec2D(x, y);

    if (not WRAPPING_ENABLED) {
        position.x = std::min(std::max(position.x, 0), TILESET_ELEMENT_SIZE);
        position.y = std::min(std::max(position.y, 0), TILESET_ELEMENT_SIZE);
    }

    TileType tile = tile_to_type[world_data[position.x][position.y]];

    if (tile == TileType::UNWALKABLE) {
        position = cached_position;
    }
    else if (tile == TileType::KILLER) {
        position = start;
    }
    float dx = TILESIZE_PIXELS * float(position.x - cached_position.x);
    float dy = TILESIZE_PIXELS * float(position.y - cached_position.y);

    move_object(id, dx, dy);

    return tile != TileType::KILLER;
}

bool Entity::walkable(Vec2D by) {
    ++call_number;
    auto new_position = position + by;
    TileType tile = tile_to_type[world_data[new_position.x][new_position.y]];
    return tile == TileType::WALKABLE;
}

void Entity::monologue() {
    std::cout << "I am " << name << " and I am standing at " << position << "!" << std::endl;
}

void Entity::run_script() {
    ++call_number;
    script(py::ptr(this));
}


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

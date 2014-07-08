#include <boost/python.hpp>
#include <boost/regex.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include "api.h"
#include "main.h"
#include "print_debug.h"

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
        move_object(id, float(start.x), float(start.y));
}

uint64_t Entity::call_number = 0;

bool Entity::move(Vec2D by) {
    ++call_number;
    auto cached_position = position;
    position += by;
    position.x = std::min(std::max(position.x,0),480);
    position.y = std::min(std::max(position.y,0),480);

    int tile_x = position.x;
    int tile_y = position.y;
    TileType tile = std::max({
        tile_to_type[world_data[(tile_x   ) / 32][(tile_y   ) / 32]],
        tile_to_type[world_data[(tile_x   ) / 32][(tile_y+31) / 32]],
        tile_to_type[world_data[(tile_x+31) / 32][(tile_y   ) / 32]],
        tile_to_type[world_data[(tile_x+31) / 32][(tile_y+31) / 32]],
    });

    if (tile == TileType::UNWALKABLE) {
        position = cached_position;
    }
    else if (tile == TileType::KILLER) {
        position = start;
    }

    move_object(id, float(position.x - cached_position.x), float(position.y - cached_position.y));
    return tile != TileType::KILLER;
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
    std::string from_file =
        // TODO: find a more scalable approach
        "import time\n"
        "def move(x):\n"
        "    for _ in range(32):\n"
        "        if not entity.move(x):\n"
        "            time.sleep(1)\n"
        "            return False\n"
        "        time.sleep(0.01)\n"
        "    return True\n"
    
        "def monologue():\n"
        "    entity.monologue()\n"
    
        "north, south, east, west = Vec2D(0, 1), Vec2D(0, -1), Vec2D(1, 0), Vec2D(-1, 0)\n"
    
        "def script(entity):\n"
        "    " + read_file();

    print_debug << from_file << std::endl;
    script = py::exec(from_file.c_str(), tempoary_scope);
    script = tempoary_scope["script"];
   // py::import("dis").attr("dis")(script);
}

std::string Entity::read_file() {
    std::string loc = "python_embed/" + name + ".py";

    //open the input file
    std::ifstream in_file(loc);

    if (in_file.is_open()) { 
        std::stringstream strStream;

        //read the file
        strStream << in_file.rdbuf();
        std::string old_text = strStream.str();
        boost::regex replace("\\n");
        std::string new_text = boost::regex_replace(old_text, replace, "\n    ");
        return new_text;
    }
    else {
        print_debug << "file opening unsuccessful" << std::endl;
        return "";
    }
}

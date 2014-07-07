#include <boost/python.hpp>
#include <boost/regex.hpp>
#include <thread>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "api.h"
#include "debug.h"
#include "main.h"

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



Player::Player(Vec2D start, std::string name, int id):
    start(start), position(start), script(""), id(id) {
        this->name = std::string(name);
        move_object(id, TILESIZE_PIXELS * float(start.x), TILESIZE_PIXELS * float(start.y));
}

uint64_t Player::call_number = 0;

bool Player::move(Vec2D by) {
    ++call_number;
    auto cached_position = position;
    position += by;
    if (not WRAPPING_ENABLED) {
        position.x = std::min(std::max(position.x,0),TILESET_ELEMENT_SIZE);
        position.y = std::min(std::max(position.y,0),TILESET_ELEMENT_SIZE);
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
        "import time\n"
        "def move(x):\n"
        "    player.move(x)\n"
        "    time.sleep(0.3)\n"
        "    return True\n"
    
        "def monologue():\n"
        "    player.monologue()\n"
    
        "north, south, east, west = Vec2D(0, 1), Vec2D(0, -1), Vec2D(1, 0), Vec2D(-1, 0)\n"
    
        "def script(player):\n"
        "    " + read_file();

    print_debug << from_file << std::endl;
    script = py::exec(from_file.c_str(), tempoary_scope);
    script = tempoary_scope["script"];
   // py::import("dis").attr("dis")(script);
}

std::string Player::read_file() {
    std::string loc = "python_embed/" + name + ".py";
    std::ifstream inFile (loc); //open the input file
    if (inFile.is_open()) { 
        std::stringstream strStream;
        strStream << inFile.rdbuf(); //read the file
        std::string old_text = strStream.str();
        boost::regex replace("\\n");
        std::string new_text = boost::regex_replace(old_text, replace, "\n    ");
        return new_text;
    } else {
        print_debug << "file opening unsuccessful" << std::endl;
        return "";
    }
}

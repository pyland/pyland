#include <boost/python.hpp>
#include <boost/regex.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "api.h"
#include "debug.h"
#include "main.h"

enum class TileType {
    WALKABLE,
    UNWALKABLE,
    KILLER
};

std::map<int, TileType> tile_to_type({
    {64, TileType::WALKABLE},
    {74, TileType::UNWALKABLE}
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
    position(start), script(""), id(id) {
        this->name = std::string(name);
        move_object(id, start.x, start.y);
}

uint64_t Player::call_number = 0;

void Player::move(Vec2D by) {
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
        position = Vec2D(0, 0);
    }

    move_object(id, position.x - cached_position.x, position.y - cached_position.y);
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
        "    for _ in range(32):\n"
        "        player.move(x)\n"
        "        time.sleep(0.01)\n"
    
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

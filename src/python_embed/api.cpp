#include "python_embed_headers.hpp"

#include <boost/python.hpp>
#include <boost/regex.hpp>
#include <fstream>
#include <future>
#include <glog/logging.h>
#include <iostream>
#include <sstream>
#include <string>

#include "api.hpp"
#include "engine_api.hpp"
#include "event_manager.hpp"
#include "gil_safe_future.hpp"

namespace py = boost::python;

Vec2D::Vec2D(int x, int y): x(x), y(y) {}

Vec2D Vec2D::operator+(Vec2D other) {
    return Vec2D(x + other.x, y + other.y);
}

Vec2D Vec2D::operator-(Vec2D other) {
    return Vec2D(x - other.x, y - other.y);
}

Vec2D Vec2D::operator/(Vec2D other) {
    return Vec2D(x / other.x, y / other.y);
}

Vec2D Vec2D::operator*(Vec2D other) {
    return Vec2D(x * other.x, y * other.y);
}

bool Vec2D::operator==(Vec2D other) {
    return (x==other.x) && (y == other.y);
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

    auto id = this->id;
    return GilSafeFuture<bool>::execute(
        [id, x, y] (GilSafeFuture<bool> walk_succeeded_return) {
            Engine::move_sprite(id, Vec2D(x, y), walk_succeeded_return);
        },
        false
    );
}

bool Entity::walkable(int x, int y) {
    ++call_number;

    auto id = this->id;
    return GilSafeFuture<bool>::execute(
        [id, x, y] (GilSafeFuture<bool> walk_succeeded_return) {
            walk_succeeded_return.set(
                Engine::walkable(Engine::find_object(id) + Vec2D(x, y))
            );
        },
        false
    );
}

void Entity::monologue() {
    auto id = this->id;
    auto name = this->name;
    return GilSafeFuture<void>::execute([id, name] (GilSafeFuture<void>) {
        std::string text = (
            "I am " + name + " and "
            "I am standing at " + Engine::find_object(id).to_string() + "!"
        );

        Engine::print_dialogue(name, text);
    });
}


// Not thread safe for efficiency reasons...
void Entity::py_print_debug(std::string text) {
    LOG(INFO) << text;
}

void Entity::py_print_dialogue(std::string text) {
    auto name = this->name;
    return GilSafeFuture<void>::execute([name, text] (GilSafeFuture<void>) {
        Engine::print_dialogue(name, text);
    });
}

void Entity::__set_game_speed(float game_seconds_per_real_second) {
    return GilSafeFuture<void>::execute([game_seconds_per_real_second] (GilSafeFuture<void>) {
        EventManager::get_instance().time.game_seconds_per_real_second = game_seconds_per_real_second;
    });
}

void Entity::py_update_status(std::string status){
    Engine::update_status(id,status);
}

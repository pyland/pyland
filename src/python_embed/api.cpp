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

    // Must be moved; cannot be copied.
    // Lambdas don't support move-capture, so
    // have to bind to the lambda.
    auto walk_succeeded_promise_ptr = std::make_shared<std::promise<bool>>();
    auto walk_succeeded_future = walk_succeeded_promise_ptr->get_future();

    auto id = this->id;
    EventManager::get_instance().add_event([walk_succeeded_promise_ptr, id, x, y] () {
        Engine::move_object(id, Vec2D(x, y), walk_succeeded_promise_ptr);
    });

    LOG(INFO) << "Getting future";

    auto m_thread_state = PyEval_SaveThread();
    auto ret = walk_succeeded_future.get();
    PyEval_RestoreThread(m_thread_state);

    LOG(INFO) << "Got future";
    return ret;
}

bool Entity::walkable(int x, int y) {
    ++call_number;

    // Must be moved; cannot be copied.
    // Lambdas don't support move-capture, so
    // have to bind to the lambda.
    auto walkable_promise_ptr = std::make_shared<std::promise<bool>>();
    auto walkable_future = walkable_promise_ptr->get_future();

    auto id = this->id;
    EventManager::get_instance().add_event([walkable_promise_ptr, id, x, y] () {
        walkable_promise_ptr->set_value(
            Engine::walkable(Engine::find_object(id) + Vec2D(x, y))
        );
    });

    LOG(INFO) << "Getting future";

    auto m_thread_state = PyEval_SaveThread();
    auto ret = walkable_future.get();
    PyEval_RestoreThread(m_thread_state);

    LOG(INFO) << "Got future";
    return ret;
}

void Entity::monologue() {
    auto id = this->id;
    auto name = this->name;
    EventManager::get_instance().add_event([id, name] () {
        std::string text = (
            "I am " + name + " and "
            "I am standing at " + Engine::find_object(id).to_string() + "!"
        );

        Engine::print_dialogue(name, text);
    });
}


void Entity::py_print_debug(std::string text) {
    EventManager::get_instance().add_event([text] () {
        LOG(INFO) << text;
    });
}

void Entity::py_print_dialogue(std::string text) {
    auto name = this->name;
    EventManager::get_instance().add_event([name, text] () {
        Engine::print_dialogue(name, text);
    });
}

void Entity::__set_game_speed(float game_seconds_per_real_second) {
    EventManager::get_instance().add_event([game_seconds_per_real_second] () {
        EventManager::get_instance().time.game_seconds_per_real_second = game_seconds_per_real_second;
    });
}

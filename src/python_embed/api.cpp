#include "python_embed_headers.hpp"

#include <boost/python.hpp>
#include <boost/regex.hpp>
#include <fstream>
#include <future>
#include <glog/logging.h>
#include <glm/vec2.hpp>
#include <iostream>
#include <sstream>
#include <string>

#include "api.hpp"
#include "engine.hpp"
#include "event_manager.hpp"
#include "gil_safe_future.hpp"

namespace py = boost::python;

Entity::Entity(glm::vec2 start, std::string name, int id):
    start(start), script(""), id(id), call_number(0) {
        this->name = std::string(name);
}

bool Entity::move(int x, int y) {
    ++call_number;

    auto id = this->id;
    return GilSafeFuture<bool>::execute(
        [id, x, y] (GilSafeFuture<bool> walk_succeeded_return) {
            Engine::move_sprite(id, glm::ivec2(x, y), walk_succeeded_return);
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
                // TODO: assert integral
                Engine::walkable(glm::ivec2(Engine::find_object(id)) + glm::ivec2(x, y))
            );
        },
        false
    );

    // TODO (Joshua): Support:
    //
    //     return GilSafeFuture<bool>::execute_get([id, x, y] () {
    //         return Engine::walkable(glm::ivec2(Engine::find_object(id)) + glm::ivec2(x, y));
    //     });
    //
}

void Entity::monologue() {
    auto id = this->id;
    auto name = this->name;
    return GilSafeFuture<void>::execute([id, name] (GilSafeFuture<void>) {
        std::ostringstream stream;

        auto where(Engine::find_object(id));
        stream << "I am " << name << " and "
               << "I am standing at " << where.x << ", " << where.y << "!";

        Engine::print_dialogue(name, stream.str());
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
    auto id(this->id);
    return GilSafeFuture<void>::execute([id, status] (GilSafeFuture<void>) {
        Engine::update_status(id, status);
    });
}

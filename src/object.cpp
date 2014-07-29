#include "api.hpp"
#include "engine_api.hpp"
#include "entitythread.hpp"
#include "map.hpp"
#include "map_viewer.hpp"
#include "object.hpp"
#include "object_manager.hpp"

#include <glog/logging.h>
#include <string>
#include <utility>


void Object::blocked_set(std::string key, Map::Blocker value) {
    blocked_tiles.erase(key);
    blocked_tiles.insert(std::make_pair(key, value));
}


Object::Object() {
    // Get a new id for the object
    ObjectManager& object_manager = ObjectManager::get_instance();
    object_manager.get_next_id(this);

    // Starting positions should be integral
    assert(trunc(x_position) == x_position);
    assert(trunc(y_position) == y_position);

    blocked_set("stood on", Engine::get_map_viewer()->get_map()->block_tile(
        Vec2D(int(x_position), int(y_position))
    ));
}

void Object::set_x_position(int x_pos) {
    x_position = double(x_pos);
}

void Object::set_x_position(double x_pos) {
    x_position = x_pos;
}

void Object::set_y_position(int y_pos) {
    y_position = double(y_pos);
}

void Object::set_y_position(double y_pos) {
    y_position = y_pos;
}

void Object::set_id(int new_id) {
    id = new_id;
}

void Object::set_name(std::string new_name) {
    name = new_name;
}

void Object::set_script(std::string new_script) {
    script = new_script;
}

void Object::set_state_on_moving_start(Vec2D target) {
    moving = true;

    // Must erase before inserting, else nothing happens
    blocked_set("walking on", Engine::get_map_viewer()->get_map()->block_tile(target));
    blocked_set("walking off", blocked_tiles.at("stood on"));
    blocked_tiles.erase("stood on");
}

void Object::set_state_on_moving_finish() {
    moving = false;
    
    blocked_set("stood on", blocked_tiles.at("walking on"));
    blocked_tiles.erase("walking on");
    blocked_tiles.erase("walking off");
}

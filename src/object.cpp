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




Object::Object() {
    // Get a new id for the object
    ObjectManager& object_manager = ObjectManager::get_instance();
    object_manager.get_next_id(this);

    // Starting positions should be integral
    assert(trunc(x_position) == x_position);
    assert(trunc(y_position) == y_position);
}
Object::~Object() {

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




#include <glog/logging.h>
#include <string>
#include <utility>

#include "api.hpp"
#include "engine.hpp"
#include "entitythread.hpp"
#include "map.hpp"
#include "map_viewer.hpp"
#include "object.hpp"
#include "object_manager.hpp"

Object::Object(): Object("") {}

Object::Object(std::string name): name(name) {
    // Get a new id for the object
    ObjectManager& object_manager = ObjectManager::get_instance();

    // WTF: Why does a getter explicitly mutate
    object_manager.get_next_id(this);

    // TODO: Maybe add the object to the object manager here
}

Object::~Object() {
    std::cout << "DESTROYING " << name << std::endl;
}

void Object::set_id(int new_id) {
    id = new_id;
}

void Object::set_name(std::string new_name) {
    name = new_name;
}

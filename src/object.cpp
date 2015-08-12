#include <glog/logging.h>
#include <string>

#include "object.hpp"
#include "entitythread.hpp"
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
    LOG(INFO) << "OBJECT DESTROYING (" << id << ")  " << name << std::endl;
}

void Object::set_id(int new_id) {
    id = new_id;
}

void Object::set_name(std::string new_name) {
    name = new_name;
}

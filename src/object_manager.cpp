#include <glog/logging.h>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <utility>

#include "object.hpp"
#include "object_manager.hpp"


ObjectManager &ObjectManager::get_instance() {
    //Lazy instantiation of the global instance
    static ObjectManager global_instance;
    return global_instance;
}

int ObjectManager::next_object_id(1);

// WTF: Mutate *and* return?
int ObjectManager::get_next_id(Object* const object) {
    //make this thread safe
    std::lock_guard<std::mutex> lock(object_manager_mutex);
    object->set_id(next_object_id);
    //Return the next object id
    return ObjectManager::next_object_id++;
}

bool ObjectManager::is_valid_object_id(int id) {
    return 0 < id && id < ObjectManager::next_object_id;
}

// WTF: Errors?
bool ObjectManager::add_object(std::shared_ptr<Object> new_object) {
    if(!new_object) {
        LOG(ERROR) << "ObjectManager::add_object: Object cannot be null.";
        return false;
    }

    int object_id = new_object->get_id();
    if(!is_valid_object_id(object_id)) {
        LOG(ERROR) << "ObjectManager::add_object: Object id is invalid; id: " << object_id;
        return false;
    }

    objects[object_id] = new_object;


    VLOG(1) << "Object " << new_object->get_id() << " added";
    return true;
}

void ObjectManager::remove_object(int object_id) {
    if (objects.count(object_id)>0) {
        objects.erase(object_id);
        VLOG(1) << "Object " << object_id << " removed";
    } else {
        VLOG(1) << "trying to remove object that either doesn't exist or there are multiple";
    }
}

void ObjectManager::print_debug() {
    std::cout <<" OBJECT MANAGER:: " << std::endl;
    for(std::pair<int, std::shared_ptr<Object>> object_pair : objects) {
        std::cout << "OBJECT ("  << object_pair.first << ") " << object_pair.second->get_name();
        std::cout << " REF COUNT: " << object_pair.second.use_count() << std::endl;
    }
    std::cout << "DONE." << std::endl;
}

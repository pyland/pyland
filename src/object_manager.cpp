#include "object_manager.hpp"

#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

ObjectManager& ObjectManager::get_instance() {
    //Lazy instantiation of the global instance
    static ObjectManager global_instance;

    return global_instance;
    
}
int ObjectManager::next_object_id(0);
int ObjectManager::get_next_id() {
    //make this thread safe
    std::lock_guard<std::mutex> lock(object_manager_mutex);

    //Return the next object id
    return ObjectManager::next_object_id++;
}

bool ObjectManager::is_valid_object_id(int id) {
    //Let 0 be an invalid object id
    if(id > 0 && id < ObjectManager::next_object_id)
        return true;
    else
        return false;
}

bool ObjectManager::add_object(std::shared_ptr<Object> new_object) {
 
    if(!new_object) {
        std::cerr << "Object cannot be NULL in ObjectManager::add_object" << std::endl;
        return false;
    }
  
    int object_id = new_object->get_id();
    if(!is_valid_object_id(object_id)) {
        std::cerr << "Object id is invalid in ObjectManager::add_object, id: " << object_id << std::endl;
        return false;
    }
  
  
    objects[object_id] = new_object;
    return true;
}

void ObjectManager::remove_object(int object_id) {
    objects.erase(object_id);
}

std::shared_ptr<Object> ObjectManager::get_object(int object_id) {

    if(!is_valid_object_id(object_id)) {
        std::cerr << "Object id is invalid in ObjectManager::get_object, id: " << object_id << std::endl;
        return nullptr;
    }
  
    //If the object isn't in the database
    if(objects.find(object_id) == objects.end()) {
        return nullptr;
    }

  
    return objects[object_id];
}

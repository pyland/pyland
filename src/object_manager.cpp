#include "object.hpp"
#include "object_manager.hpp"
#include <glog/logging.h>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

ObjectManager& ObjectManager::get_instance() {
    //Lazy instantiation of the global instance
    static ObjectManager global_instance;

    return global_instance;
   
}
int ObjectManager::next_object_id(1);

int ObjectManager::get_next_id(Object* const object) {
    //make this thread safe
    std::lock_guard<std::mutex> lock(object_manager_mutex);
object->set_id(next_object_id);
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
        LOG(ERROR) << "ObjectManager::add_object: Object cannot be null.";
        return false;
    }
  
    int object_id = new_object->get_id();
    if(!is_valid_object_id(object_id)) {
        LOG(ERROR) << "ObjectManager::add_object: Object id is invalid; id: " << object_id;
        return false;
    }
  
  
    objects[object_id] = new_object;
    return true;
}

void ObjectManager::remove_object(int object_id) {
    objects.erase(object_id);
}

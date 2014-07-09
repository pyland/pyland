#include "object_manager.hpp"

#include <iostream>

bool ObjectManager::is_valid_object_id(int id) {

  //Let -1 be an invalid object id
  if(id > 0)
    return true;
  else
    return false;
}

bool ObjectManager::add_object(Object* new_object) {
 
  if(new_object == NULL) {
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

Object* ObjectManager::get_object(int object_id) {

  if(!is_valid_object_id(object_id)) {
    std::cerr << "Object id is invalid in ObjectManager::get_object, id: " << object_id << std::endl;
    return NULL;
  }
  
  //If the object isn't in the database
  if(objects.find(object_id) == objects.end()) {
    return NULL;
  }

  
  return objects[object_id];
}

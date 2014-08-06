#include <glog/logging.h>
#include <iostream>
#include <memory>

template <typename R>
std::shared_ptr<R> ObjectManager::get_object(int object_id) {
    if(!is_valid_object_id(object_id)) {
        LOG(ERROR) << "ObjectManager::get_object: Object id is invalid; id: " << object_id;
        return nullptr;
    }
  
    //If the object isn't in the database
    if(objects.find(object_id) == objects.end()) {
        return nullptr;
    }

    //Returns null if the object is not of the required type
    return std::dynamic_pointer_cast<R>(objects[object_id]);
}

#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

#include "object.hpp"
#include <map>

///
/// This class holds the database of all the objects in the game. It
/// manages the objects and is used to unload them.
///
class ObjectManager {

  ///
  /// The collection of all the objects the manager is currently managing
  /// Stored using the object id as the identifier
  /// 
  std::map<int, Object*> objects;

  ///
  /// The number of objects the object manager is currently managing
  ///
  int num_objects  = 0;

  ///
  /// Function to check if a given object id is valid
  /// @param id the identifier to check
  /// @return boolean value which is true if the identifier is valid and false if not
  ///
  static bool is_valid_object_id(int id);

 public:

  ///
  /// Add an object to the object manager to manage
  ///
  bool add_object(Object* new_object);
  /// 
  /// Get an object from the object manager
  /// @return the requested object
  ///
  Object* get_object(int object_id);
};

#endif

#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

#include <map>
#include <memory>
#include <mutex>

class Object;

///
/// This class holds the database of all the objects in the game. It
/// manages the objects and is used to unload them.
/// Object ids start at 1. 0 indicates an invalid object
///
class ObjectManager {

    ///
    /// The object manager isn't designed to be thread safe but, i
    /// want to lock the generation of ids so that this bit is
    ///
    std::mutex object_manager_mutex;

    ///
    /// The id for the next object to be added to the object manager
    /// to manage. Starts at 1. An invalid object is indicated by an
    /// id of 0.
    ///
    static int next_object_id;

    ///
    /// The collection of all the objects the manager is currently managing
    /// Stored using the object id as the identifier
    ///
    std::map<int, std::shared_ptr<Object>> objects;

    ObjectManager() {};
    ~ObjectManager() {};
public:
    ///
    /// Function to check if a given object id is valid
    /// @param id the identifier to check
    /// @return boolean value which is true if the identifier is valid and false if not
    ///
    static bool is_valid_object_id(int id);


    ///
    /// Getter for the main global object manger. This is the manager
    /// used by the majority of the engine
    /// You cannot have more than one global object manager
    ///
    static ObjectManager &get_instance();

    ///
    /// Gets a new, globally unique id for an object. Starts at  as 0
    /// indicates an invalid identifier for an object
    /// @param object the object to set the id for
    ///
    int get_next_id(Object* const object);

    ///
    /// Add an object to the object manager to manage
    ///
    bool add_object(std::shared_ptr<Object> new_object);

    ///
    /// Remoe an object from the object manager
    /// @param object_id the identifier of the object
    ///
    void remove_object(int object_id);

    ///
    /// Get an object from the object manager
    /// @return the requested object
    ///
    template <typename R>
    std::shared_ptr<R> get_object(int object_id);

    ///
    /// Prints debug information
    ///
    void print_debug();
};

#include "object_manager.hxx"

#endif

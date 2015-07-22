#ifndef API_H
#define API_H

#include <boost/python/base_type_traits.hpp>
#include <boost/python/object_core.hpp>
#include <boost/python/list.hpp>
#include <stdint.h>

#include <glm/vec2.hpp>
#include <string>

#include<iostream>
namespace py = boost::python;

///
/// Player object passable to Python after wrapping.
///
class Entity {
    private:
        ///
        /// Starting postiton for this entity, used to reset location when entity dies.
        ///
        glm::vec2 start;


    public:
        ///
        /// A name. The name is used to identify instances of the object in the python
        /// code.
        ///
        std::string name;
        
        ///
        /// Where the object is in the filesystem relative to game/objects, (where it's python code and sprites are stored!!!!!
        ///
        std::string file_location;

        ///
        /// ID of entity, referring to in-game object that the API calls are passed to.
        ///
        int id;

        ///
        /// Incrementing counter used to check if an API call has been made
        /// since the last check.
        ///
        uint64_t call_number;

        void callback_test(PyObject *callback);

        ///
        /// Construct Entity with a given place, name and id.
        ///
        /// @param start
        ///     Inital position, in tiles from bottom-left.
        ///     TODO: replace so as to not require vector displacements.
        ///
        /// @param name
        ///     A name.
        ///
        /// @param file_location
        ///
        /// @param id
        ///     ID of entity, referring to in-game object that the API calls are passed to.
        ///
        /// @return
        ///     New instance of Entity
        ///
        Entity(glm::vec2 start, std::string name, std::string file_location, int id);

        ///
        /// Move entity relative to current location.
        ///
        /// @param x
        ///     x-displacement to shift right by, in tiles.
        ///
        /// @param y
        ///     y-displacement to shift up by, in tiles.
        ///
        /// @return
        ///     Whether move was successful.
        ///
        void move(int x, int y, PyObject *callback);

        void move_east(PyObject *callback);
        void move_south(PyObject *callback);
        void move_north(PyObject *callback);
        void move_west(PyObject *callback);
        
        ///
        /// @return
        ///     The name of the entity, this name is also used as the instance name of the entity in python. :)
        ///
        std::string get_name();

        ///
        /// @return
        ///     The game objects location in the file-system (the folder where it's python script and sprites are!)
        ///
        std::string get_location();

        ///
        /// Checks if player can move by the vector given.
        ///
        /// @param by
        ///     position representing movement in the axes.
        ///     TODO: replace so as to not require vector displacements.
        ///
        /// @return
        ///     Whether the area is walkable.
        ///
        bool walkable(int x, int y);

        ///
        /// Prints to standard output the name and position of entity.
        ///
        /// TODO: rename this method, it's a silly name!
        ///
        void monologue();

        ///
        /// Cut down any objects around the player
        ///
        /// @param x
        ///     x-displacement to shift right by, in tiles.
        ///
        /// @param y
        ///     y-displacement to shift up by, in tiles.
        ///
        /// @return
        ///     Whether cut was successful.
        ///
        bool cut(int x, int y);

        ///
        /// Centre the camera on this object. Snaps instantly
        ///
        void focus();

        ///
        /// Look for any objects in a range. Returns an array of
        /// (name, x, y) tuples
        ///
        /// @param search_range
        ///     the search range out from the player, 1 is all tiles 1 away from the player
        ///
        /// @return
        ///     Whether move was successful.
        ///
        py::list look(int search_range);

        void py_print_debug(std::string text);
        void py_print_dialogue(std::string text);

        void __set_game_speed(float game_seconds_per_real_second);

        void py_update_status(std::string status);

        std::string get_instructions();

        py::list get_retrace_steps();
        py::object read_message();
};


#endif

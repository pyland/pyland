#ifndef API_H
#define API_H

#include <boost/python/base_type_traits.hpp>
#include <boost/python/object_core.hpp>
#include <boost/python/list.hpp>
#include <boost/python/tuple.hpp>
#include <stdint.h>

#include <glm/vec2.hpp>
#include <string>
#include <iostream>

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
        std::string sprite_location;
        int current_frame;

        bool animating;

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
        /// Call the callback after a given number of in-game time-units
        ///
        void wait(double gametime, PyObject *callback);

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
        void move_by(int x, int y, double duration, PyObject *callback);

        void move_east(PyObject *callback);
        void move_south(PyObject *callback);
        void move_north(PyObject *callback);
        void move_west(PyObject *callback);


        ///
        /// @return
        ///     True if the entity is moving, false otherwise.
        bool is_moving();

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
        /// @return
        /// 	The folder location of the sprite that is currently being displayed (relative to the object's sprites folder)
        ///
        std::string get_sprite();

        ///
        /// @param sprite_location
        /// 	Location of the sprite that you want displayed (relative to the oject's sprites folder, (eg main/north))
        ///
        void set_sprite(std::string sprite_location, PyObject *callback);

        ///
        /// Make the object start animating, cycles through all the numbered png's in the sprite folder set
        ///
        ///
        void start_animating();

        ///
        /// Make the object pause on the current frame of animation
        ///
        ///
        void pause_animating();

        void animate(int current_frame);

        ///
        /// @return
        /// 	The number of animation frames the object has in their set sprite folder
        ///
        int get_number_of_animation_frames();

        ///
        /// @param frame_number
        /// 	The frame number you want to display of the object's sprite
        ///
        void set_animation_frame(int frame_number);

        ///
        /// Set the solidity of the Entity. (This is whether or not other solid MapObjects can got 'through' it)
        ///
        /// @param solidity
        ///     Whether you want the object to be solid (true) or not solid (false)
        ///
        void set_solidity(bool solidity, PyObject *callback);

        ///
        /// Return the solidity of the object
        ///
        /// @return
        ///     True if object is solid, false otherwise
        ///
        bool is_solid();

        ///
        /// Centre the camera on this object. Snaps instantly
        ///
        void focus();

        ///
        /// Returns if the object is the camera focus
        ///
        bool is_focus();

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
        void py_print_terminal(std::string text, bool error);

        void __set_game_speed(float game_seconds_per_real_second);

        void py_update_status(std::string status);

        std::string get_instructions();

        py::list get_retrace_steps();
        py::object read_message();

        ///
        /// Get the id of the Entity. (The id being what is used to look up instances of the MapObject)
        ///
        /// @return
        ///     The id of the MapObject the Entity is associated with.
        ///
        int get_id();

        ///
        /// Get the position of the Entity on the map as a python tuple.
        ///
        /// @return
        ///     The position of the Entity
        ///
        py::tuple get_position();
};


#endif

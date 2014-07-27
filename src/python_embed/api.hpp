#ifndef API_H
#define API_H

#include "python_embed_headers.hpp"

#include <boost/python.hpp>
#include <string>

namespace py = boost::python;

///
/// Vector displacement.
///
class Vec2D {
    public:
        ///
        /// Tempoarily public position attributes.
        ///
        int x;
        int y;

        ///
        /// Vec2D constructor
        ///
        /// @param x
        ///     x-axis position
        ///
        /// @param y
        ///     y-axis position
        ///
        /// @return
        ///     Instance of Vec2D
        ///
        Vec2D(int x, int y);

        ///
        /// Addition of Vec2Ds, not mutating either vector
        ///
        /// @param other
        ///     Coordinate being added
        ///
        /// @return
        ///     New instance of Vec2D
        ///
        Vec2D operator+(Vec2D other);
        Vec2D operator-(Vec2D other);

        ///
        /// In-place addition of Vec2Ds
        ///
        /// @param other
        ///     The other coordinate being added
        ///
        void operator+=(Vec2D other);
        void operator-=(Vec2D other);


        ///
        /// Convert Vec2D to a std::string
        ///
        /// @return String representation of Vec2D
        ///
        std::string to_string();
};

///
/// Player object passable to Python after wrapping.
///
class Entity {
    private:
        ///
        /// Starting postiton for this entity, used to reset location when entity dies.
        ///
        Vec2D start;
        
        ///
        /// The wrapped python script.
        ///
        /// @deprecated
        ///
        boost::python::api::object script;


    public:
        ///
        /// A name. This is used to find scripts, and eventually
        /// will be a deprecated or randomly-generated feature.
        ///
        std::string name;
        
        /// 
        /// ID of entity, referring to in-game object that the API calls are passed to.
        ///
        int id;

        ///
        /// Incrementing counter used to check if an API call has been made
        /// since the last check.
        ///
        uint64_t call_number;

        ///
        /// Construct Entity with a given place, name and id.
        ///
        /// @param start
        ///     Inital position, in tiles from bottom-left.
        ///     TODO: replace so as to not require vector displacements.
        ///
        /// @param name
        ///     A name. This is used to find scripts, and eventually
        ///     will be a deprecated or randomly-generated feature.
        ///
        /// @param id
        ///     ID of entity, referring to in-game object that the API calls are passed to.
        ///
        /// @return
        ///     New instance of Entity
        ///
        Entity(Vec2D start, std::string name, int id);

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
        bool move(int x, int y);

        ///
        /// Checks if player can move by the vector given.
        ///
        /// @param by
        ///     Vec2D representing movement in the axes.
        ///     TODO: replace so as to not require vector displacements.
        ///
        /// @return
        ///     Whether the area is walkable.
        ///
        bool walkable(int x, int y);

        ///
        /// Prints to standard output the name and position of entity.
        ///
        void monologue(); 

        void py_print_debug(std::string text);
};


#endif

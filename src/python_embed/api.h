#ifndef API_H
#define API_H

#include <boost/python.hpp>
#include <string>

namespace py = boost::python;

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
        /// @param x x-axis position
        /// @param y y-axis position
        /// @return Instance of Vec2D
        ///
        Vec2D(int x, int y);

        ///
        /// Addition of Vec2Ds, not mutating either vector
        ///
        /// @param other Coordinate being added
        /// @return New instance of Vec2D
        ///
        Vec2D operator+(Vec2D other);
        Vec2D operator-(Vec2D other);

        ///
        /// In-place addition of Vec2Ds
        ///
        /// @param other The other coordinate being added
        /// @return void 
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

class Entity {
    private:
        Vec2D start;
        Vec2D position;
        std::string name;
        boost::python::api::object script;
        std::string read_file();
        int id;

    public:

        ///
        /// Incrementing counter used to check if an API call has been made
        /// since the last check. This will be replaced in order to support
        /// multiple threads.
        ///
        static uint64_t call_number;

        ///
        /// Entity constructor
        ///
        /// @param start Inital position
        /// @param name  Name of entity  
        /// @return New instance of Entity
        ///
        Entity(Vec2D start, std::string name, int id);

        ///
        /// Move entity relative to current location
        ///
        // /// @param by Vec2D representing movement in the axes
        ///
        bool move(int x, int y);

        ///
        /// Prints to standard output the name and position of sprite
        ///
        void monologue();

        ///
        /// Runs the Entity's active script
        ///
        /// @see give_script()
        ///
        void run_script();

        ///
        /// Sets entity's Python script from a hardcoded file.
        /// Takes a namespace to evaluate it in.
        /// 
        /// @param main_namespace The namespace to evaluate the file in.
        /// @see run_script()
        ///
        void give_script(boost::python::api::object in);
};

#endif

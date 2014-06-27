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

        ///
        /// In-place addition of Vec2Ds
        ///
        /// @param other The other coordinate being added
        /// @return void 
        ///
        void operator+=(Vec2D other);

        ///
        /// Convert Vec2D to a std::string
        ///
        /// @return String representation of Vec2D
        ///
        std::string to_string();
};

class Player {
    private:
        Vec2D position;
        std::string name;
        boost::python::api::object script;
        std::string read_file();

    public:

        ///
        /// Incrementing counter used to check if an API call has been made
        /// since the last check. This will be replaced in order to support
        /// multiple threads.
        ///
        static long call_number;

        ///
        /// Player constructor
        ///
        /// @param start Inital position
        /// @param name  Name of player  
        /// @return New instance of Player
        ///
        Player(Vec2D start, std::string name);

        ///
        /// Move player relative to current location
        ///
        /// @param by Vec2D representing movement in the axes
        ///
        void move(Vec2D by);

        ///
        /// Prints to standard output the name and position of sprite
        ///
        void monologue();

        ///
        /// Runs the Player's active script
        ///
        /// @see give_script()
        ///
        void run_script();

        ///
        /// Sets player's Python script from a hardcoded file.
        /// Takes a namespace to evaluate it in.
        /// 
        /// @param main_namespace The namespace to evaluate the file in.
        /// @see run_script()
        ///
        void give_script(boost::python::api::object in);
};

#include <boost/python.hpp>
#include <string>

enum class Direction {UP, DOWN, LEFT, RIGHT};


class Vec2D {
    public:
        int x;
        int y;

        /**
         *  Vec2D constructor
         *  @param x x coordinate
         *  @param y y coordinate
         *  @return instance of Vec2D
         */
        Vec2D(int x, int y);

        /**
         *  addition of Vec2Ds, not mutating either vector
         *  @param other the other coordinate being added
         *  @return new instance of Vec2D
         */
        Vec2D operator+(Vec2D other);

        /**
         *  addition of Vec2Ds, mutating the vector its called on (not other)
         *  @param other the other coordinate being added
         *  @return void 
         */
        void operator+=(Vec2D other);

        /**
         *  conversion of Vec2D to string
         *  @return string representation of Vec2D
         */
        std::string to_string();
};

class Player {
    private:
        Vec2D position;
        Direction direction;
        std::string name;
        boost::python::api::object script;

    public:
        static long call_number;
        static bool in_call;

        Player(Vec2D start, Direction direction, std::string name);
        void move(Vec2D by);
        void monologue();
        void run_script();
        void give_script(boost::python::api::object in);
};

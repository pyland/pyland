#include <boost/python.hpp>
#include <string>

enum class Direction {UP, DOWN, LEFT, RIGHT};

class Vec2D {
    public:
        int x;
        int y;
        Vec2D(int x, int y);
        Vec2D operator+(Vec2D other);
        void operator+=(Vec2D other);
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

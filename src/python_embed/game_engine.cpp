#include <glog/logging.h>

#include "game_engine.hpp"

GameEngine::GameEngine() {

}

/*
boost::python::object GameEngine::addObject(std::string name, std::string class_location, int x, int y) {
    use the challenge instance to make and object and load it into the challenge
    wrap the object an entity instance
    grab the class from the class location
    create a new instance of that class and use it to wrap the entity instance
    return that intance, that instance will also be located at the coordinates given in-game
}

boost::python::list GameEngine::get_objectsAt(int x, int y) {
    return a python list of all the correctly wrapped game-object instances at the location given.
}

void GameEngine::change_level(std::string level_location) {
    return;
}
*/

boost::python::object GameEngine::add_object(std::string name, std::string class_location, int x, int y) {
    LOG(INFO) << "Creating an instance of " << class_location << " at (" << x << ", " << y << ") called " << name;
    //use the challenge instance to make and object and load it into the challenge
    //wrap the object an entity instance
    //grab the class from the class location
    //create a new instance of that class and use it to wrap the entity instance
    //return that intance, that instance will also be located at the coordinates given in-game
    return boost::python::list();
}

std::string GameEngine::get_level_location() {
    return "test_world/test_level/test_one";
}

void GameEngine::print_debug(std::string debug_message) {
    LOG(INFO) << debug_message; // TODO: work out properly how python messages should be debugged.
}

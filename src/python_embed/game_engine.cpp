#include <glog/logging.h>

#include "game_engine.hpp"
#include "button.hpp"
#include "text_font.hpp"
#include "engine.hpp"
#include "game_main.hpp"


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

*/


void GameEngine::change_level(std::string level_location) {
    //TODO: run the finish.py script of a level.
    LOG(INFO) << "Changing level to " << level_location;
    return;
}


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


void GameEngine::add_button(std::string file_path, std::string name, int button_type, PyObject* callback) {

    boost::python::object boost_callback(boost::python::handle<>(boost::python::borrowed(callback)));

    std::shared_ptr<Button> new_button;
    if(button_type == 1){
        new_button = std::make_shared<Button>(ButtonType::Board);
    }
    else if(button_type == 2){
        new_button = std::make_shared<Button>(ButtonType::SpriteHead);
    }
    game_main->get_buttons().push_back(new_button);
    new_button->set_picture(file_path);
    new_button->set_text(name);
    new_button->set_on_click(boost_callback);
    game_main->get_sprite_window()->add(new_button);
    game_main->refresh_gui();
}

void GameEngine::register_input_callback(int input_key, PyObject *py_input_callback) {
    boost::python::object input_callback(boost::python::handle<>(boost::python::borrowed(py_input_callback)));
    InputHandler::get_instance()->register_input_callback(input_key, input_callback);
    return;

}

#include <glog/logging.h>

#include "audio_engine.hpp"
#include "button.hpp"
#include "config.hpp"
#include "engine.hpp"
#include "event_manager.hpp"
#include "game_engine.hpp"
#include "gui_main.hpp"
#include "text_font.hpp"


/*
boost::python::object GameEngine::addObject(std::string name, std::string class_location, int x, int y) {
    use the challenge instance to make and object and load it into the challenge
    wrap the object an entity instance
    grab the class from the class location
    create a new instance of that class and use it to wrap the entity instance
    return that intance, that instance will also be located at the coordinates given in-game
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
    nlohmann::json j = Config::get_instance();
    std::string map_name = j["files"]["level_location"];
    //return "test_world/test_level/test_one";
    return map_name;
}

void GameEngine::print_debug(std::string debug_message) {
    LOG(INFO) << debug_message; // TODO: work out properly how python messages should be debugged.
}


unsigned int GameEngine::add_button(std::string file_path, std::string name, PyObject* callback) {
    //TODO: Find a way of avoiding this hack
    LOG(INFO) << "Adding a new button: " << name;
    boost::python::object boost_callback(boost::python::handle<>(boost::python::borrowed(callback)));
    //Uniquely identify each button to it's associated with the python player.
    //This identifier is mapped to the deque index (in gui_main) at which the player's focus button
    //is stored
    button_id++;
    unsigned int id_to_pass = button_id;
    EventManager::get_instance()->add_event([this, file_path, name, boost_callback, id_to_pass] {
        gui_main->add_button(file_path, name, boost_callback, id_to_pass);
    });
    return button_id;
}

void GameEngine::set_cur_player(unsigned int passing_button_id){
    EventManager::get_instance()->add_event([this, passing_button_id] {
       gui_main->click_player(passing_button_id);
    });
    return;
}

void GameEngine::update_player_name(std::string name, unsigned int passing_button_id) {
    EventManager::get_instance()->add_event([this, name, passing_button_id] {
       gui_main->update_button_text(name, passing_button_id);
    });
    return;
}

void GameEngine::register_input_callback(int input_key, PyObject *py_input_callback) {
    boost::python::object input_callback(boost::python::handle<>(boost::python::borrowed(py_input_callback)));
    InputHandler::get_instance()->register_input_callback(input_key, input_callback);
    return;
}

void GameEngine::play_music(std::string song_name) {
    AudioEngine::get_instance()->play_music("../game/music/" + song_name + ".ogg");
}

void GameEngine::print_terminal(std::string text, bool error) {
    Engine::print_terminal(text, error);
}

boost::python::list GameEngine::get_objects_at(int x, int y) {
    std::vector<int> object_ids = Engine::get_objects_at(glm::ivec2(x, y));
    boost::python::list python_list;
    //Go through the vector of object_ids and append them to the python list
    for(auto object_id: object_ids) {
        python_list.append(object_id);
    }
    return python_list;
}

void GameEngine::set_ui_colours(int r1, int b1, int g1, int r2, int b2, int g2){
    Engine::set_ui_colours(r1,b1,g1,r2,b2,g2);
}

void GameEngine::set_running(){
    Engine::set_running();
}

void GameEngine::set_finished(){
    Engine::set_finished();
}

void GameEngine::trigger_run(){
    //Passing 0 as the script causes it to run the currently open script
    Engine::trigger_run(0);
}

int GameEngine::get_run_script(){
    return Engine::get_run_script();
}

bool GameEngine::is_solid(int x, int y) {
    return !Engine::walkable(glm::ivec2(x, y)); //TODO: Make syntax of Engine match this!
}




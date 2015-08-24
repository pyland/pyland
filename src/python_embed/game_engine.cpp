#include <glog/logging.h>
#include <deque>

#include "audio_engine.hpp"
#include "button.hpp"
#include "challenge.hpp"
#include "challenge_data.hpp"
#include "config.hpp"
#include "engine.hpp"
#include "event_manager.hpp"
#include "game_engine.hpp"
#include "gui_main.hpp"
#include "map_loader.hpp"
#include "map.hpp"
#include "text_font.hpp"

GameEngine::GameEngine(GUIMain *_gui_main, Challenge *_challenge){
    gui_main = _gui_main;
    challenge = _challenge;
    button_id = 0;
}


void GameEngine::change_map(std::string map_location) {
    Engine::change_map(map_location);
    LOG(INFO) << "Changing level to " << map_location;
    return;
}

int GameEngine::get_tile_type(int x, int y) {
    return Engine::get_tile_type(x, y);
}


boost::python::object GameEngine::create_object(std::string object_file_location, std::string object_name, int x, int y) {
    LOG(INFO) << "Creating an instance of " << object_file_location << " at (" << x << ", " << y << ") called " << object_name;
    Entity *entity = challenge->create_entity(object_name, object_file_location, glm::ivec2(x, y)); //For some reason this freezes the game when called from here. does it still do that?
    return boost::python::api::object(boost::ref(*entity));
}

std::string GameEngine::get_level_location() {
    //Config::json j = Config::get_instance();
    //std::string map_name = j["files"]["level_location"];
    //return "test_world/test_level/test_one";
    std::string map_name = challenge->challenge_data->level_location;//"test_world/test_level/test_one";//challenge->challenge_data->map_name;
    LOG(INFO) << "Getting map " << std::endl;
    return map_name;
}

void GameEngine::print_debug(std::string debug_message) {
    LOG(INFO) << debug_message; // TODO: work out properly how python messages should be debugged.
}

void GameEngine::show_dialogue(std::string text, bool disable_scripting, PyObject *callback) {

    if(Engine::is_bar_open()){
        LOG(INFO) << "Replacing the old notification bar with the new one";
        Engine::close_notification_bar();
    }

    boost::python::object boost_callback(boost::python::handle<>(boost::python::borrowed(callback)));

    LOG(INFO) << "Adding \"" << text << "\"to the notification bar with a regular callback";
    Engine::add_text(text);
    Engine::open_notification_bar(disable_scripting, boost_callback);
}

void GameEngine::show_dialogue_with_options(std::string text, bool disable_scripting, PyObject *_boost_options){

    if(Engine::is_bar_open()){
        LOG(INFO) << "Replacing the old notification bar with the new one";
        Engine::close_notification_bar();
    }

    boost::python::dict boost_options(boost::python::handle<>(boost::python::borrowed(_boost_options)));

    std::deque<std::pair<std::string, std::function<void ()> > > options;
    boost::python::list option_names = boost_options.keys();

    for(int i=0; i<len(option_names); i++){
        boost::python::extract<std::string> extracted_option_name(option_names[i]);
        boost::python::object extracted_callback = boost_options[option_names[i]];

        //boost::python::extract is dodgy, so we need to have these intermediate variables for
        //implicit type casting
        std::string cpp_option_name = extracted_option_name;

        auto test = [extracted_callback] () { extracted_callback(); };

        options.push_back(std::make_pair(cpp_option_name, test));
    }

    LOG(INFO) << "Adding \"" << text << "\"to the notification bar with options";
    Engine::add_text(text);
    Engine::open_notification_bar_with_options(disable_scripting, options);
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

void GameEngine::flush_input_callback_list(int input_key) {
    InputHandler::get_instance()->flush_list(input_key);
    return;
}

void GameEngine::play_music(std::string song_name) {
    AudioEngine::get_instance()->play_music("../game/music/" + song_name + ".ogg");
}


void GameEngine::show_py_scripter(PyObject* callback){
    boost::python::object boost_callback(boost::python::handle<>(boost::python::borrowed(callback)));
    Engine::show_py_scripter();
    EventManager::get_instance()->add_event([boost_callback] {
       boost_callback();
    });
}

void GameEngine::hide_py_scripter(PyObject* callback){
    boost::python::object boost_callback(boost::python::handle<>(boost::python::borrowed(callback)));
    Engine::hide_py_scripter();
    EventManager::get_instance()->add_event([boost_callback] {
       boost_callback();
    });
}

void GameEngine::enable_py_scripter(PyObject* callback){
    boost::python::object boost_callback(boost::python::handle<>(boost::python::borrowed(callback)));
    Engine::enable_py_scripter();
    EventManager::get_instance()->add_event([boost_callback] {
       boost_callback();
    });
}

void GameEngine::disable_py_scripter(PyObject* callback){
    boost::python::object boost_callback(boost::python::handle<>(boost::python::borrowed(callback)));
    Engine::disable_py_scripter();
    EventManager::get_instance()->add_event([boost_callback] {
       boost_callback();
    });
}

void GameEngine::enable_script_editing(PyObject* callback){
    boost::python::object boost_callback(boost::python::handle<>(boost::python::borrowed(callback)));
    Engine::enable_script_editing();
    EventManager::get_instance()->add_event([boost_callback] {
       boost_callback();
    });
}

void GameEngine::disable_script_editing(PyObject* callback){
    boost::python::object boost_callback(boost::python::handle<>(boost::python::borrowed(callback)));
    Engine::disable_script_editing();
    EventManager::get_instance()->add_event([boost_callback] {
       boost_callback();
    });
}

void GameEngine::set_py_tabs(int val, PyObject* callback){
    boost::python::object boost_callback(boost::python::handle<>(boost::python::borrowed(callback)));
    Engine::set_py_tabs(val);
    EventManager::get_instance()->add_event([boost_callback] {
       boost_callback();
    });
}

void GameEngine::show_external_tab(PyObject* callback){
    boost::python::object boost_callback(boost::python::handle<>(boost::python::borrowed(callback)));
    Engine::show_external_tab();
    EventManager::get_instance()->add_event([boost_callback] {
       boost_callback();
    });
}

void GameEngine::hide_external_tab(PyObject* callback){
    boost::python::object boost_callback(boost::python::handle<>(boost::python::borrowed(callback)));
    Engine::hide_external_tab();
    EventManager::get_instance()->add_event([boost_callback] {
       boost_callback();
    });
}


void GameEngine::update_world(std::string text){
  Engine::update_world(text);
}

void GameEngine::update_level(std::string text){
  Engine::update_level(text);
}

void GameEngine::update_coins(int value){
  Engine::update_coins(value);
}


void GameEngine::update_totems(int value, bool show){
   Engine::update_totems(value, show);
}

void GameEngine::insert_to_scripter(std::string text)
{
    Engine::insert_to_scripter(text);
}

void GameEngine::clear_scripter(PyObject* callback)
{
    boost::python::object boost_callback(boost::python::handle<>(boost::python::borrowed(callback)));
    Engine::clear_scripter();
    EventManager::get_instance()->add_event([boost_callback] {
       boost_callback();
    });
}

std::string GameEngine::get_script()
{
    return Engine::get_script();
}

void GameEngine::print_terminal(std::string text, bool error) {
    Engine::print_terminal(text, error);
}

std::string GameEngine::get_config() {
    Config::json j = Config::get_instance();
    return j.dump();
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

void GameEngine::refresh_config() {
    Config::refresh_config();
}

void GameEngine::add_event(PyObject *py_callback) {
    boost::python::object callback(boost::python::handle<>(boost::python::borrowed(py_callback)));
    EventManager::get_instance()->add_event(callback);
    return;
}

std::string GameEngine::get_player_name() {
    return Engine::get_player_name();
}

void GameEngine::set_player_name(std::string player_name) {
    Engine::set_player_name(player_name);
}




#include "interpreter.hpp"

#include <QDataStream>
#include <QMetaType>
#include <QTextStream>
#include <QCursor>
#include <QStyleOption>
#include <qcoreevent.h>

#include <glog/logging.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <glm/vec2.hpp>
#include <iostream>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <thread>
#include <tuple>
#include <vector>

#include "dispatcher.hpp"
#include "engine.hpp"
#include "event_manager.hpp"
#include "game_main.hpp"
#include "game_time.hpp"
#include "mainwindow.h"
#include "map.hpp"
#include "map_object.hpp"
#include "map_viewer.hpp"
#include "object_manager.hpp"
#include "text.hpp"
#include "text_box.hpp"

///Static variables
MapViewer *Engine::map_viewer(nullptr);

std::shared_ptr<TextBox> Engine::notification_bar(nullptr);
GameMain* Engine::game_main(nullptr);

GameWindow* Engine::game_window(nullptr);
MainWindow* Engine::main_window(nullptr);
GUIMain* Engine::gui_main(nullptr);
Challenge* Engine::challenge(nullptr);
int Engine::tile_size(64);
float Engine::global_scale(1.0f);

void Engine::move_object(int id, glm::ivec2 move_by) {
    // TODO: Make sure std::promise garbage collects correctly
    Engine::move_object(id, move_by, 0.3,  [] () {});
}

// Undefined for diagonals, but will return within correct half
std::string to_direction(glm::ivec2 direction) {
    // ╲•╱
    // •╳•
    // ╱•╲
    if (direction.x > direction.y) {
        // ╲ ╱
        //  ╳•
        // ╱•╲
        if (direction.x > -direction.y) {
            // ╲ ╱
            //  ╳•
            // ╱ ╲
            return "east";
        }
        else {
            // ╲ ╱
            //  ╳
            // ╱•╲
            return "south";
        }
    }
    else {
        // ╲•╱
        // •╳
        // ╱ ╲
        if (direction.x > -direction.y) {
            // ╲•╱
            //  ╳
            // ╱ ╲
            return "north";
        }
        else {
            // ╲ ╱
            // •╳
            // ╱ ╲
            return "west";
        }
    }
}

//TODO: This needs to work with renderable objects
void Engine::move_object(int id, glm::ivec2 move_by, double duration, std::function<void ()> func) {
    auto object(ObjectManager::get_instance().get_object<MapObject>(id));

    if (!object || object->is_moving()) {
        EventManager::get_instance()->add_event(func); //Even if object can't move, but callback on event queue
        return;
    }

    // Get the object's game position
    glm::vec2 location(object->get_game_position());
    auto target(location);
    target += move_by;

    VLOG(2) << "Trying to walk to " << target.x << " " << target.y;

    // check if object is solid to know if it cares about solidity of other objects
    if (object->get_walkability() == Walkability::WALKABLE) {
        VLOG(2) << "ignore if walkable or not";
    } else {
        if (!walkable(target)) { target = location; }
    }

    object->set_state_on_moving_start(target);

    // Step-off events
    get_map_viewer()->get_map()->event_step_off.trigger(location, id);

    std::string direction(to_direction(move_by));

    // Motion
    EventManager::get_instance()->add_timed_event(
        GameTime::duration(duration),
        [direction, move_by, location, target, id, func] (float completion) mutable {
            auto object = ObjectManager::get_instance().get_object<MapObject>(id);
            if (!object) { return false; }

            // Long rambly justification about how Ax + B(1-x) can be outside
            // the range [A, B] (consider when A=B).
            //
            // The given formula cannot have this problem when A and B are exactly representable
            glm::vec2 tweened_position(location + completion * (target-location));

            object->set_game_position(target); //Set the object's game position to be where it will be!
            object->set_render_position(tweened_position);

            if (completion == 1.0) {
                object->set_state_on_moving_finish();

                // TODO: Make this only focus if the object
             #include <thread>   // is the main object.
                if (Engine::map_viewer) {
                    Engine::map_viewer->refocus_map();
                }

                // Step-on events
                get_map_viewer()->get_map()->event_step_on.trigger(target, id);

                EventManager::get_instance()->add_event(func);
            }

            // Run to completion
            return true;
        }
    );
}

bool Engine::walkable(glm::ivec2 location) {
    int map_width = map_viewer->get_map()->get_width();
    int map_height = map_viewer->get_map()->get_height();

    // Check bounds
    if(!(0 <= location.x && location.x < map_width) || !(0 <= location.y && location.y < map_height)) {
        VLOG(2) << "Cannot move to requested tile due to map bounds";
        return false;
    }

    // Check against collidable layer
    if(!Engine::map_viewer->get_map()->is_walkable(location.x, location.y)) {
        VLOG(2) << "Cannot move to requested tile due to collidable objects";
        return false;
    }

    //check for tile blockers
    if (get_map_viewer()->get_map()->blocker.at(location.x).at(location.y) != 0) {
        VLOG(2) << "Cannot move to requested tile due to tile blocker";
        return false;
    }

    return true;
}

void Engine::change_map(std::string map_location){
    game_main->change_challenge(map_location);
}

void Engine::change_tile(glm::ivec2 tile, std::string layer_name, std::string tile_name) {
    map_viewer->get_map()->update_tile(tile.x, tile.y, layer_name, tile_name);
}

static std::vector<int> location_filter_objects(glm::ivec2 location, std::vector<int> objects) {
    auto &object_manager(ObjectManager::get_instance());

    std::vector<int> results;
    std::copy_if(std::begin(objects), std::end(objects), std::back_inserter(results),
        [&] (int object_id) {
            auto object(object_manager.get_object<MapObject>(object_id));
            return object && object->get_game_position() == location;
        }
    );
    return results;
}

std::vector<int> Engine::get_objects_at(glm::ivec2 location) {
    return location_filter_objects(location, map_viewer->get_map()->get_objects());
}

MainWindow* Engine::get_main_window(){
    return main_window;
}

// TODO: Consider whether finding the object and checking its position is saner
bool Engine::is_object_at(glm::ivec2 location, int object_id) {
    auto objects(get_objects_at(location));

    return std::any_of(std::begin(objects), std::end(objects), [&] (int id) {
        return id == object_id;
    });
}

bool Engine::is_objects_at(glm::ivec2 location, std::vector<int> object_ids) {
    return std::all_of(std::begin(object_ids), std::end(object_ids), [&] (int object_id) {
        return is_object_at(location, object_id);
    });
}

int Engine::get_tile_type(int x, int y) {
    return map_viewer->get_map()->get_tile_type(x, y);
}

void Engine::add_dialogue(std::string text) {

    EventManager::get_instance()->add_event([text] {
        gui_main->add_message(text);
    });
}

void Engine::add_text(std::string text) {

    EventManager::get_instance()->add_event([text] {
        gui_main->add_text(text);
    });
}

void Engine::open_notification_bar(std::function<void ()> func){
    disable_py_scripter();
    EventManager::get_instance()->add_event([func] {
        gui_main->open_notification_bar(func);
    });

}

void Engine::open_notification_bar_with_options(std::deque<std::pair <std::string, std::function<void ()> > > options){
    disable_py_scripter();
    EventManager::get_instance()->add_event([options] {
        gui_main->open_notification_bar_with_options(options);
    });

}

void Engine::close_notification_bar(){

    EventManager::get_instance()->add_event([] {
        gui_main->close_notification_bar();
        enable_py_scripter();
    });

}

bool Engine::is_bar_open(){
    return (gui_main->is_bar_open());
}

void Engine::show_py_scripter(){
    auto _main_window = main_window;
    EventManager::get_instance()->add_event([_main_window] {
        _main_window->showScripterPanel();
    });
}

void Engine::hide_py_scripter(){
    auto _main_window = main_window;
    EventManager::get_instance()->add_event([_main_window] {
        _main_window->hideScripterPanel();
    });
}

void Engine::enable_py_scripter(){
    auto _main_window = main_window;
    EventManager::get_instance()->add_event([_main_window] {
        _main_window->enableScripterPanel();
    });
}

void Engine::disable_py_scripter(){
    auto _main_window = main_window;
    EventManager::get_instance()->add_event([_main_window] {
        _main_window->disableScripterPanel();
    });
}

void Engine::enable_script_editing(){
    auto _main_window = main_window;
    EventManager::get_instance()->add_event([_main_window] {
        _main_window->enableScripter();
    });
}

void Engine::disable_script_editing(){
    auto _main_window = main_window;
    EventManager::get_instance()->add_event([_main_window] {
        _main_window->disableScripter();
    });
}

void Engine::set_py_tabs(int val){
    auto _main_window = main_window;
    EventManager::get_instance()->add_event([_main_window, val] {
        _main_window->setTabs(val);
    });
}

void Engine::update_world(std::string text){
    auto _main_window = main_window;
    EventManager::get_instance()->add_event([_main_window, text] {
         _main_window->setWorld(text);
    });
}

void Engine::update_level(std::string text){
    auto _main_window = main_window;
    EventManager::get_instance()->add_event([_main_window, text] {
          _main_window->setLevel(text);
    });
}

void Engine::update_coins(int value){
    auto _main_window = main_window;
    EventManager::get_instance()->add_event([_main_window, value] {
          _main_window->setCoins(value);
    });
}


void Engine::update_totems(int value,bool show){
    auto _main_window = main_window;
    EventManager::get_instance()->add_event([_main_window, value, show] {
          _main_window->setCurTotems(value, show);
    });
}

void Engine::insert_to_scripter(std::string text)
{
    auto _main_window = main_window;
    EventManager::get_instance()->add_event([_main_window, text] {
        _main_window->insertToTextEditor(text);
    });
}

void Engine::clear_scripter()
{
    auto _main_window = main_window;
    EventManager::get_instance()->add_event([_main_window] {
        _main_window->clearTextEditor();
    });
}

std::string Engine::get_script()
{
    return main_window->getEditorText();
}


//Print to the QT terminal widget
void Engine::print_terminal(std::string text, bool error) {
    auto _main_window = main_window;
    EventManager::get_instance()->add_event([_main_window, text, error] {
        _main_window->setAnyOutput(true);
        _main_window->pushTerminalText(text, error);
    });

}


//Focuses on the next player that has a focus button for it
void Engine::focus_next() {
    auto _game_main = game_main;
    EventManager::get_instance()->add_event([_game_main] {
        _game_main->focus_next();
    });
}


void Engine::set_any_output(bool option){
    auto _main_window = main_window;
    EventManager::get_instance()->add_event([_main_window, option] {
        _main_window->setAnyOutput(option);
    });
}

void Engine::set_ui_colours(int r1, int b1, int g1, int r2, int b2, int g2){
    auto _main_window = main_window;
    EventManager::get_instance()->add_event([_main_window, r1, b1, g1, r2, b2, g2] {
        _main_window->setColourScheme(r1,b1,g1,r2,b2,g2);
    });
}

void Engine::set_running(){
    auto _main_window = main_window;
    EventManager::get_instance()->add_event([_main_window] {
        _main_window->setRunning(true);
    });
}

void Engine::set_finished(){
    auto _main_window = main_window;
    EventManager::get_instance()->add_event([_main_window] {
        _main_window->setRunning(false);
    });
}

void Engine::trigger_run(int script){
    auto _main_window = main_window;
    EventManager::get_instance()->add_event([_main_window, script] {
        _main_window->runCode(script);
    });
}

std::string Engine::get_player_name(){
    return game_main->player_name;
}


void Engine::set_player_name(std::string player_name){
    auto _game_main = game_main;
    EventManager::get_instance()->add_event([_game_main, player_name] {
        _game_main->player_name = player_name;
    });
}


void Engine::trigger_speed(){
    auto _main_window = main_window;
    EventManager::get_instance()->add_event([_main_window] {
        _main_window->toggleSpeed();
    });
}

int Engine::get_run_script(){
    return main_window->getExecuteScript();
}

TextFont Engine::get_game_font() {
    return TextFont(get_game_typeface(), 19);
}

Typeface Engine::get_game_typeface() {
    Config::json j;
    j = Config::get_instance();
    std::string typeface_location = j["files"]["dialogue_font"];
    return Typeface(typeface_location);
}

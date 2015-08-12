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

    // Position should be integral at this point
    glm::vec2 location(object->get_position());
    auto target(location);
    target += move_by;

    VLOG(2) << "Trying to walk to " << target.x << " " << target.y;

    // animate walking in-place TODO: It doesn't actually seem to do this, work out what it does!
    auto MapObject_test(ObjectManager::get_instance().get_object<MapObject>(id));
    if (!MapObject_test) {
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

            object->set_position(tweened_position);

            if (completion == 1.0) {
                object->set_state_on_moving_finish();

                // TODO: Make this only focus if the object
                // is the main object.
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

void Engine::change_tile(glm::ivec2 tile, std::string layer_name, std::string tile_name) {
    map_viewer->get_map()->update_tile(tile.x, tile.y, layer_name, tile_name);
}


glm::vec2 Engine::find_object(int id) {
    Map *map = CHECK_NOTNULL(CHECK_NOTNULL(map_viewer)->get_map());

    //Check the object is on the map
    auto objects = map->get_objects();
    for(auto object_id : objects) {
        if(object_id == id) {
            //MapObject is on the map so now get its location
            auto object = ObjectManager::get_instance().get_object<MapObject>(id);
            return object->get_position();
        }
    }

    //Not on the map
    throw std::runtime_error("MapObject is not in the map");
}

static std::vector<int> location_filter_objects(glm::vec2 location, std::vector<int> objects) {
    auto &object_manager(ObjectManager::get_instance());

    std::vector<int> results;
    std::copy_if(std::begin(objects), std::end(objects), std::back_inserter(results),
        [&] (int object_id) {
            auto object(object_manager.get_object<MapObject>(object_id));
            return object && object->get_position() == location;
        }
    );
    return results;
}

std::vector<int> Engine::get_objects_at(glm::vec2 location) {
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

    EventManager::get_instance()->add_event([func] {
        gui_main->open_notification_bar(func);
    });

}

void Engine::close_notification_bar(){

    EventManager::get_instance()->add_event([] {
        gui_main->close_notification_bar();
    });

}

bool Engine::is_bar_open(){
	return (gui_main->is_bar_open());
}

//Print to the QT terminal widget
void Engine::print_terminal(std::string text, bool error) {
    main_window->setAnyOutput(true);
    main_window->pushTerminalText(text,error);
}


//Focuses on the next player that has a focus button for it
void Engine::focus_next() {
    game_main->focus_next();
    return;
}


void Engine::set_any_output(bool option){
    main_window->setAnyOutput(option);
}

void Engine::set_ui_colours(int r1, int b1, int g1, int r2, int b2, int g2){
    main_window->setColourScheme(r1,b1,g1,r2,b2,g2);
}

void Engine::set_running(){
    main_window->setRunning(true);
}

void Engine::set_finished(){
    main_window->setRunning(false);
}

void Engine::trigger_run(int script){
    main_window->runCode(script);
}

void Engine::trigger_speed(){
    main_window->toggleSpeed();
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

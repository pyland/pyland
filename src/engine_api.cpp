#include "engine_api.hpp"
#include "map_viewer.hpp"
#include "object.hpp"
#include "object_manager.hpp"
#include "dispatcher.hpp"

#include <cstdlib>
#include <glog/logging.h>
#include <memory>
#include <utility>
#include <vector>
#include <iostream>


bool Engine::move_object(int id, Vec2D move_by) {

    std::shared_ptr<Object> object = ObjectManager::get_instance().get_object<Object>(id);

    if(object) {
        //Check if a move can be performed
        Vec2D new_loco = Vec2D(object->get_x_position() + move_by.x, object->get_y_position() + move_by.y);
        VLOG(2) << "Trying to walk to " << new_loco.x << " " << new_loco.y << ".\n"
                << "Tile blocker count is " << get_map_viewer()->get_map()->blocker.at(new_loco.x).at(new_loco.y);
        if ((!walkable(new_loco)) || (get_map_viewer()->get_map()->blocker.at(new_loco.x).at(new_loco.y) != 0)) {
            return false;
        } else {
            // trigger any waiting events on leaving 
            Vec2D leave_tile = Vec2D(object->get_x_position(), object->get_y_position());
            get_map_viewer()->get_map()->event_step_off.trigger(leave_tile, id);

            //TODO, make this an event movement
            object->set_x_position(object->get_x_position() + move_by.x);
            object->set_y_position(object->get_y_position() + move_by.y);

            //trigger any waiting events on arriving
            Vec2D arrive_tile = Vec2D(object->get_x_position(), object->get_y_position());
            get_map_viewer()->get_map()->event_step_on.trigger(arrive_tile, id);

            //if there is a map viewer attached
            if(Engine::map_viewer != nullptr) {
                //animate the map if this is the object to focus ong
                Engine::map_viewer->refocus_map();
            }
        }
    }
    return true;
}

MapViewer* Engine::map_viewer = nullptr;

bool Engine::walkable(Vec2D location) {
    int map_width = map_viewer->get_map()->get_width();
    int map_height = map_viewer->get_map()->get_height();

    //Check bounds
    if(location.x < 0 || location.x >= map_width || location.y < 0 || location.y >= map_height) {
        return false;
    }
       
    //Check for collidable objects
    if(!Engine::map_viewer->get_map()->is_walkable(location.x, location.y))
        return false;

    return true;
}

bool Engine::change_tile(int, Vec2D, int) {
    return false;
}

std::vector<int> Engine::get_tiles(Vec2D) {
    std::vector<int> tiles;
    return tiles;
}

int Engine::get_tile_size() {
    throw std::runtime_error("get_tile_size not supported");
}

std::vector<int> Engine::get_objects(Vec2D) {
    std::vector<int> objects;
    return objects;
}

bool Engine::load_map(int) {
    return false;
}


Vec2D Engine::find_object(int id) {
    if (!Engine::map_viewer) {
        throw std::runtime_error("Object Requested could not be found");
    }

    Map* map = map_viewer->get_map();
    if (!map) {
        throw std::runtime_error("Object Requested could not be found");
    }

    //Check the object is on the map
    auto objects = map->get_characters();
    for(auto object_id : objects) {
        if(object_id == id) {
            //Object is on the map so now get its locationg
            auto object = ObjectManager::get_instance().get_object<Object>(id);
            return Vec2D(object->get_x_position(), object->get_y_position());
        }
    }

    //Not on the map
    throw std::runtime_error("Object is not in the map");
}

bool Engine::open_editor(std::string editor, std::string filename) {
    LOG(INFO) << "Opening editor";
    //TODO remove this function in the final version
    std::string bash_command = editor + std::string(" python_embed/scripts/") + filename;
    std::thread TEXT_EDIT([] (std::string command) { system(command.c_str()); }, bash_command);
    TEXT_EDIT.detach();
    return true;
}

std::vector<int> get_objects_at(Vec2D) {
    // TODO
    throw std::runtime_error("get_objects_at not implemented");
}

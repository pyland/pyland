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


bool Engine::move_object(int id, int tile_dx, int tile_dy) {

    std::shared_ptr<Object> object = ObjectManager::get_instance().get_object<Object>(id);

    if(object) {
        //Check if a move can be performed
        if (!walkable(object->get_x_position() + tile_dx, object->get_y_position() + tile_dy)) {
            return false;
        } else {
            // trigger any waiting events on leaving 
            std::pair<int,int> leave_tile = std::make_pair(object->get_x_position(), object->get_y_position());
            get_map_viewer()->get_map()->event_step_off.trigger(leave_tile, id);

            //TODO, make this an event movement
            object->set_x_position(object->get_x_position() + tile_dx);
            object->set_y_position(object->get_y_position() + tile_dy);

            //trigger any waiting events on arriving
            std::pair<int,int> arrive_tile = std::make_pair(object->get_x_position(), object->get_y_position());
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

bool Engine::walkable(int x_pos, int y_pos) {
    int map_width = map_viewer->get_map()->get_width();
    int map_height = map_viewer->get_map()->get_height();

    //Check bounds
    if(x_pos < 0 || x_pos >= map_width || y_pos < 0 || y_pos >= map_height) {
        return false;
    }
       
    //Check for collidable objects
    if(!Engine::map_viewer->get_map()->is_walkable(x_pos, y_pos))
        return false;

    return true;
}

bool Engine::change_tile(int, int, int, int) {
    return false;
}

std::vector<int> Engine::get_tiles(int, int) {
    std::vector<int> tiles;
    return tiles;
}

int Engine::get_tile_size() {
    return -1; // TODO
}

std::vector<int> Engine::get_objects(int, int) {
    std::vector<int> objects;
    return objects;
}

bool Engine::load_map(int) {
    return false;
}


std::pair<int, int> Engine::find_object(int id) {

    std::string exception_str = "Object Requested could not be found";

    if(Engine::map_viewer == nullptr)
        throw exception_str;

    Map* map = map_viewer->get_map();
    if(map == nullptr)
        throw exception_str;
    
    //Check the object is on the map
    auto objects = map->get_characters();
    for(auto object_id : objects) {
        if(object_id == id) {
            //Object is on the map so now get its locationg
            auto object = ObjectManager::get_instance().get_object<Object>(id);
            return std::make_pair<int, int>(object->get_x_position(), object->get_y_position());
        }
    }

    //Not on the map
    throw exception_str;
}

bool Engine::open_editor(std::string editor, std::string filename) {
    LOG(INFO) << "Opening editor";
    //TODO remove this function in the final version
    std::string bash_command = editor + std::string(" python_embed/scripts/") + filename;
    std::thread TEXT_EDIT([] (std::string command) { system(command.c_str()); }, bash_command);
    TEXT_EDIT.detach();
    return true;
}

std::vector<int> get_objects_at(int /*x_pos*/, int /*y_pos*/) {
    // TODO
    throw std::runtime_error("get_objects_at not implemented");
}

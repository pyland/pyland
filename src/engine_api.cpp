#include "engine_api.hpp"
#include "object.hpp"
#include "object_manager.hpp"

#include <memory>
#include <utility>
#include <vector>

bool Engine::move_object(int id, int tile_dx, int tile_dy) {
    
    std::shared_ptr<Object> object = ObjectManager::get_instance().get_object(id);
    if(object) {
        //Check if a move can be performed
        if(!walkable(object->get_x_position() + tile_dx, object->get_y_position() + tile_dy))
            return false;

        //TODO, make this an event movement
        object->set_x_position((int)(object->get_x_position() + (int)tile_dx));
        object->set_y_position((int)(object->get_y_position() + (int)tile_dy));
    }
    return true;
}

bool Engine::walkable(int x_pos, int y_pos) {

    //TODO: make this walkable work
    return true;
}
bool Engine::change_tile(int new_id, int x, int y, int layer) {
    return false;
}

std::vector<int> Engine::get_tiles(int x, int y) {
    std::vector<int> tiles;
    return tiles;
}

std::vector<int> Engine::get_objects(int x, int y) {
    std::vector<int> objects;
    return objects;
}

std::pair<int, int> Engine::find_object(int id) {
    std::pair<int, int> position;
    return position;
}

bool Engine::load_map(int map_id) {
    return false;
}

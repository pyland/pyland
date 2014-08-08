#include <cstdlib>
#include <future>
#include <glog/logging.h>
#include <memory>
#include <utility>
#include <vector>
#include <iostream>

#include "engine_api.hpp"
#include "event_manager.hpp"
#include "game_time.hpp"
#include "gil_safe_future.hpp"
#include "map_viewer.hpp"
#include "object.hpp"
#include "object_manager.hpp"
#include "dispatcher.hpp"
#include "sprite.hpp"

///Static variables
MapViewer* Engine::map_viewer = nullptr;
Text* Engine::dialogue_box = nullptr;
int Engine::tile_size= 16;
int Engine::global_scale = 2;
Notification Engine::notifcation_stack = Notification();


//TODO: THis needs to work with renderable objects

void Engine::move_sprite(int id, Vec2D move_by) {
    // TODO: Make sure std::promise garbage collects correctly
    Engine::move_sprite(id, move_by, GilSafeFuture<bool>());
}

//TODO: This needs to work with renderable objects
void Engine::move_sprite(int id, Vec2D move_by, GilSafeFuture<bool> walk_succeeded_return) {

    auto sprite = ObjectManager::get_instance().get_object<Sprite>(id);

    if (!sprite || sprite->is_moving()) { return; }

    Vec2D location(int(sprite->get_x_position()), int(sprite->get_y_position()));
    Vec2D target = location + move_by;

    VLOG(2) << "Trying to walk to " << target.x << " " << target.y;
    VLOG(2) << "Tile blocker count is " << get_map_viewer()->get_map()->blocker.at(target.x).at(target.y);

    // TODO: animate walking in-place
    if (!walkable(target)) { return; }

    sprite->set_state_on_moving_start(target);

    // Step-off events
    EventManager::get_instance().add_event([location, id] () {
        get_map_viewer()->get_map()->event_step_off.trigger(location, id);
    });

    // move text with charcters
    // Vec2D pixel_target = get_map_viewer()->get_map()->tile_to_pixel(target);
    // sprite->get_sprite_text()->move(pixel_target.x, pixel_target.y);

    // Motion
    EventManager::get_instance().add_timed_event(
        GameTime::duration(0.07),
        [walk_succeeded_return, location, target, id] (double completion) mutable {
            auto sprite = ObjectManager::get_instance().get_object<Sprite>(id);
            if (!sprite) { return false; }

            double x_position = location.x * (1-completion) + target.x * completion;
            double y_position = location.y * (1-completion) + target.y * completion;
            sprite->set_x_position(x_position);
            sprite->set_y_position(y_position);

            if (completion == 1.0) {
                sprite->set_state_on_moving_finish();

                // TODO: Make this only focus if the sprite
                // is the main sprite.
                if (Engine::map_viewer) {
                    Engine::map_viewer->refocus_map();
                }

                // Step-on events
                EventManager::get_instance().add_event([target, id] () {
                    get_map_viewer()->get_map()->event_step_on.trigger(target, id);
                });

                walk_succeeded_return.set(true);
            }

            // Run to completion
            return true;
        }
    );
}

bool Engine::walkable(Vec2D location) {
    int map_width = map_viewer->get_map()->get_width();
    int map_height = map_viewer->get_map()->get_height();

    //Check bounds
    if(location.x < 0 || location.x >= map_width || location.y < 0 || location.y >= map_height) {
        VLOG(2) << "Cannot move to requested tile due to map bounds";
        return false;
    }

    //Check for collidable objects
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

bool Engine::change_tile(Vec2D tile, int layer_num, int tile_id) {
    map_viewer->get_map()->update_tile(tile.x, tile.y, layer_num, tile_id);
    return true;
}

std::vector<int> Engine::get_tiles(Vec2D) {
    std::vector<int> tiles;
    return tiles;
}

std::vector<int> Engine::get_objects(Vec2D) {
    std::vector<int> objects;
    return objects;
}
std::vector<int> Engine::get_sprites(Vec2D) {
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
    auto map_objects = map->get_map_objects();
    auto sprites = map->get_sprites();
    for(auto object_id : map_objects) {
        if(object_id == id) {
            //Object is on the map so now get its location
            auto object = ObjectManager::get_instance().get_object<MapObject>(id);
            return Vec2D(int(object->get_x_position()),
                         int(object->get_y_position()));
        }
    }

    for(auto object_id : sprites) {
        if(object_id == id) {
            //Object is on the map so now get its location
            auto object = ObjectManager::get_instance().get_object<Sprite>(id);
            return Vec2D(int(object->get_x_position()),
                         int(object->get_y_position()));
        }
    }
    //Not on the map
    throw std::runtime_error("Object is not in the map");
}

bool Engine::open_editor(std::string filename) {
    LOG(INFO) << "Opening editor";
    //TODO remove this function in the final version
    std::string bash_command = editor + std::string(" python_embed/scripts/") + filename;
    std::thread TEXT_EDIT([] (std::string command) { system(command.c_str()); }, bash_command);
    TEXT_EDIT.detach();
    return true;
}


std::vector<int> Engine::get_objects_at(Vec2D location) {
    Map* map = map_viewer->get_map();
    if (!map) {
        throw std::runtime_error("Map not avalaible");
    }

    std::vector<int> results;
    //Check the object is on the map

    auto map_objects = map->get_map_objects();
    for(int object_id : map_objects) {
        //Object is on the map so now get its locationg
        auto object = ObjectManager::get_instance().get_object<MapObject>(object_id);
        Vec2D current_location(int(object->get_x_position()),
                               int(object->get_y_position()));

        if (current_location == location) {
            results.push_back(object_id);
        }
    }

    return results;
}

std::vector<int> Engine::get_sprites_at(Vec2D location) {
    Map* map = map_viewer->get_map();
    if (!map) {
        throw std::runtime_error("Map not avalaible");
    }

    std::vector<int> results;
    //Check the object is on the map

    auto sprites = map->get_sprites();
    for(int object_id : sprites) {
        //Object is on the map so now get its locationg
        auto object = ObjectManager::get_instance().get_object<Sprite>(object_id);
        Vec2D current_location(int(object->get_x_position()),
                               int(object->get_y_position()));

        if (current_location == location) {
            results.push_back(object_id);
        }
    }

    return results;
}

std::string Engine::editor = DEFAULT_PY_EDITOR;

void Engine::print_dialogue(std::string name, std::string text) {
    std::string text_to_display = name + " : " + text;
    notifcation_stack.add_new(text_to_display);
    EventManager::get_instance().add_event(
        [text_to_display] () {
            Engine::get_dialogue_box()->set_text(text_to_display);
            std::cout << text_to_display << std::endl;
        }
    );
}

void Engine::move_notification(Direction direction) {
    switch (direction) {
        case (Next):
            Engine::get_dialogue_box()->set_text(notifcation_stack.forward());
            break;
        case (Previous):
            Engine::get_dialogue_box()->set_text(notifcation_stack.backward());
            break;
    }
}

void Engine::text_displayer() {

    Map* map = map_viewer->get_map();
    if (!map) {
        throw std::runtime_error("Map not avalaible");
    }

    auto objects = map->get_sprites();
    for(int object_id : objects) {
        //Object is on the map so now get its locationg
        auto sprite = ObjectManager::get_instance().get_object<Sprite>(object_id);
        if (sprite->get_object_text() != nullptr) {
            sprite->get_object_text()->display();
        }
        if (sprite->get_status_text() != nullptr) {
            sprite->get_status_text()->display();
        }
    }
}

void Engine::text_updater() {
    Map* map = map_viewer->get_map();
    if (!map) {
        throw std::runtime_error("Map not avalaible");
    }

    auto objects = map->get_sprites();
    for(int object_id : objects) {
        //Object is on the map so now get its locationg
        auto sprite = ObjectManager::get_instance().get_object<Sprite>(object_id);

        std::pair<double,double> tile(sprite->get_x_position(), sprite->get_y_position());

        Vec2D pixel_position = Engine::get_map_viewer()->tile_to_pixel(tile);

        //VLOG(2) << "sprite location" << sprite->get_x_position() << " " << sprite->get_y_position();
        //VLOG(2) << "Pixel position: " << pixel_position.to_string();

        sprite->get_object_text()->move(
            pixel_position.x + (int)(0.5*Engine::get_actual_tile_size()), 
            pixel_position.y
        );
        sprite->get_status_text()->move(
            pixel_position.x + (int)(0.5*Engine::get_actual_tile_size()), 
            pixel_position.y + (int)(1.5*Engine::get_actual_tile_size())
        );
    }

}

void Engine::update_status(int id, std::string status) {
    auto sprite = ObjectManager::get_instance().get_object<Sprite>(id);
    sprite->get_status_text()->set_text(status);
}


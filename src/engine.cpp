#include <glog/logging.h>
#include <algorithm>
#include <cstdlib>
#include <glm/vec2.hpp>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <thread>
#include <vector>

#include "dispatcher.hpp"
#include "engine.hpp"
#include "event_manager.hpp"
#include "game_time.hpp"
#include "gil_safe_future.hpp"
#include "map.hpp"
#include "map_object.hpp"
#include "map_viewer.hpp"
#include "notification_bar.hpp"
#include "object_manager.hpp"
#include "gil_safe_future.hpp"
#include "sprite.hpp"
#include "text.hpp"


///Static variables
MapViewer *Engine::map_viewer(nullptr);
NotificationBar *Engine::notification_bar(nullptr);
GameWindow* Engine::game_window(nullptr);
int Engine::tile_size(64);
float Engine::global_scale(1.0f);



void Engine::move_sprite(int id, glm::ivec2 move_by) {
    // TODO: Make sure std::promise garbage collects correctly
    Engine::move_sprite(id, move_by, GilSafeFuture<bool>());
}

//TODO: This needs to work with renderable objects
void Engine::move_sprite(int id, glm::ivec2 move_by, GilSafeFuture<bool> walk_succeeded_return) {

    auto sprite(ObjectManager::get_instance().get_object<Sprite>(id));

    if (!sprite || sprite->is_moving()) { return; }

    // Position should be integral at this point
    glm::vec2 target(sprite->get_position());
    auto location(target);
    target += move_by;

    VLOG(2) << std::setprecision(15) << "Trying to walk to " << target.x << " " << target.y;

    // TODO: animate walking in-place
    if (!walkable(target)) { return; }

    sprite->set_state_on_moving_start(target);

    // Step-off events
    get_map_viewer()->get_map()->event_step_off.trigger(location, id);

    // Motion
    EventManager::get_instance().add_timed_event(
        GameTime::duration(0.14),
        [walk_succeeded_return, location, target, id] (float completion) mutable {
            auto sprite = ObjectManager::get_instance().get_object<Sprite>(id);
            if (!sprite) { return false; }

            glm::vec2 tweened_position(location);
            if (location.x != target.x) {
                tweened_position.x = location.x * (1-completion) + target.x * completion;
            }
            if (location.y != target.y) {
                tweened_position.y = location.y * (1-completion) + target.y * completion;
            }




            sprite->set_position(tweened_position);
   

            if (completion == 1.0) {
                sprite->set_state_on_moving_finish();

                // TODO: Make this only focus if the sprite
                // is the main sprite.
                if (Engine::map_viewer) {
                    Engine::map_viewer->refocus_map();
                }

                // Step-on events
                get_map_viewer()->get_map()->event_step_on.trigger(target, id);

                walk_succeeded_return.set(true);
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

void Engine::change_tile(glm::ivec2 tile, int layer_num, std::string tile_name) {
    map_viewer->get_map()->update_tile(tile.x, tile.y, layer_num, tile_name);
}


glm::vec2 Engine::find_object(int id) {
    Map *map = CHECK_NOTNULL(CHECK_NOTNULL(map_viewer)->get_map());

    //Check the object is on the map
    auto map_objects = map->get_map_objects();
    auto sprites = map->get_sprites();
    for(auto object_id : map_objects) {
        if(object_id == id) {
            //Object is on the map so now get its location
            auto object = ObjectManager::get_instance().get_object<MapObject>(id);
            return object->get_position();
        }
    }

    for(auto object_id : sprites) {
        if(object_id == id) {
            //Object is on the map so now get its location
            auto object = ObjectManager::get_instance().get_object<Sprite>(id);
            return object->get_position();
        }
    }

    //Not on the map
    throw std::runtime_error("Object is not in the map");
}

void Engine::open_editor(std::string filename) {
    LOG(INFO) << "Opening editor";

    //TODO remove this function in the final version
    std::string command(editor + std::string(" python_embed/scripts/") + filename);

    // TODO: Make this close safely.
    std::thread([command] () { system(command.c_str()); }).detach();
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
    return location_filter_objects(location, map_viewer->get_map()->get_map_objects());
}

// TODO: Also return MapObjects
std::vector<int> Engine::get_sprites_at(glm::vec2 location) {
    return location_filter_objects(location, map_viewer->get_map()->get_sprites());
}

// TODO: Consider whether finding the object and checking its position is saner
bool Engine::is_object_at(glm::ivec2 location, int object_id) {
    auto objects(get_objects_at(location));

    return std::any_of(std::begin(objects), std::end(objects), [&] (int id) {
        return id == object_id;
    });
}

std::string Engine::editor = DEFAULT_PY_EDITOR;

void Engine::print_dialogue(std::string name, std::string text) {
    std::string text_to_display = name + " : " + text;
    notification_bar->add_notification(text_to_display);
}


void Engine::text_displayer() {
    Map *map = CHECK_NOTNULL(map_viewer->get_map());

    auto objects = map->get_sprites();
    for (int object_id : objects) {
        //Object is on the map so now get its locationg
        auto sprite = ObjectManager::get_instance().get_object<Sprite>(object_id);
        if (sprite->get_object_text()) {
            sprite->get_object_text()->display();
        }
    }
}

void Engine::text_updater() {
    Map *map = CHECK_NOTNULL(map_viewer->get_map());

    auto objects = map->get_sprites();
    for (int object_id : objects) {
        //Object is on the map so now get its location
        auto sprite = ObjectManager::get_instance().get_object<Sprite>(object_id);

        glm::ivec2 pixel_position(Engine::get_map_viewer()->tile_to_pixel(sprite->get_position()));

        sprite->get_object_text()->move(
            pixel_position.x + int(Engine::get_actual_tile_size() / 2.0f),
            pixel_position.y
        );
    }

}

void Engine::update_status(int id, std::string status) {
    auto sprite = ObjectManager::get_instance().get_object<Sprite>(id);
    sprite->set_sprite_status(status);
}

TextFont Engine::get_game_font() {
    return TextFont(get_game_typeface(), 19);
}

Typeface Engine::get_game_typeface() {
    return Typeface("../fonts/Ubuntu-R.ttf");
}

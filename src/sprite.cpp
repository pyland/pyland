#include <new>
#include <glog/logging.h>
#include <iostream>
#include <fstream>

#ifdef USE_GLES
#include <GLES2/gl2.h>
#endif

#ifdef USE_GL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif

#include "image.hpp"
#include "engine.hpp"
#include "entitythread.hpp"
#include "map_viewer.hpp"
#include "renderable_component.hpp"
#include "sprite.hpp"
#include "object_manager.hpp"
#include "walkability.hpp"


Sprite::Sprite(glm::ivec2 position,
               std::string name,
               Walkability walkability,
               int sheet_id,
               std::string sheet_name):

    MapObject(position, name, walkability, sheet_id, sheet_name) {

        auto map_viewer(Engine::get_map_viewer());

        // WTF: why is text here?
        // TODO: Serious spring cleaning
        // Setting up sprite text
        Typeface mytype("../fonts/hans-kendrick/HansKendrick-Regular.ttf");
        TextFont myfont(mytype, 18);

        // TODO: Smart pointer
        object_text = new Text(Engine::get_map_viewer()->get_window(), myfont, true);
        object_text->set_text(name);
        glm::ivec2 pixel_position(map_viewer->tile_to_pixel(position));
        object_text->move(pixel_position.x, pixel_position.y);

        object_text->resize(100,100);
        object_text->align_centre();
        object_text->align_at_origin(true);
        LOG(INFO) << "Setting up text at " << pixel_position.x << ", " << pixel_position.y;

        // setting up status text
        status_text = new Text(map_viewer->get_window(), myfont, true);
        status_text->set_text("awaiting...");
        glm::ivec2 pixel_text(map_viewer->tile_to_pixel(position));
        status_text->move(pixel_text.x, pixel_text.y + 100);

        status_text->resize(100,100);
        status_text->align_centre();
        status_text->align_at_origin(true);

        // TODO: Starting positions should be integral as of currently. Check or fix.
        //
        // Make a map object blocked
        // In future this might not be needed
        blocked_tiles.insert(std::make_pair(
            "stood on",
            Engine::get_map_viewer()->get_map()->block_tile(position)
        ));

        /// build focus icon
        LOG(INFO) << "setting up focus icon";
        focus_icon = std::make_shared<MapObject>(position, "focus icon", Walkability::WALKABLE, 96);
        ObjectManager::get_instance().add_object(focus_icon);
        auto focus_icon_id(focus_icon->get_id());
        LOG(INFO) << "created focus icon with id: " << focus_icon_id;
        Engine::get_map_viewer()->get_map()->add_map_object(focus_icon_id);

        LOG(INFO) << "Sprite initialized";
}

Sprite::~Sprite() {
    // TODO: Smart pointers
    delete object_text;
    delete status_text;
    LOG(INFO) << "Sprite destructed";
}

void Sprite::set_state_on_moving_start(glm::ivec2 target) {
    moving = true;
    // adding blocker to new tile
    blocked_tiles.insert(std::make_pair("walking to", Engine::get_map_viewer()->get_map()->block_tile(target)));
}

void Sprite::set_state_on_moving_finish() {
    moving = false;
    // removing old blocker and changing key for new one
    blocked_tiles.erase("stood on");
    blocked_tiles.insert(std::make_pair("stood on", blocked_tiles.at("walking to")));
    blocked_tiles.erase("walking to");
}

void Sprite::add_to_inventory(std::shared_ptr<MapObject> new_object) {
    LOG(INFO) << "adding item to sprites inventory";
    new_object->set_position(position);
    inventory.push_back(new_object);
}

void Sprite::set_position(glm::vec2 position) {
    MapObject::set_position(position);

    for (auto item : get_inventory()) {
        item->set_position(position);
    }

    focus_icon->set_position(position);
}

bool Sprite::remove_from_inventory(std::shared_ptr<MapObject> old_object) {
    // there must be a better way to do this
    auto it = std::find(std::begin(inventory), std::end(inventory), old_object);
    if (it != std::end(inventory)) {
        inventory.erase(it);
        LOG(INFO) << "removing item to sprites inventory";
        return true;
    } else {
        return false;
    }
}

bool Sprite::is_in_inventory(std::shared_ptr<MapObject> object) {
    auto it = std::find(std::begin(inventory), std::end(inventory), object);
    return it != std::end(inventory);
}


Sprite_Status Sprite::string_to_status(std::string status) {
    std::map<std::string,Sprite_Status> string_map = {
        { "",        Sprite_Status::NOTHING },
        { "failed",  Sprite_Status::FAILED  },
        { "killed",  Sprite_Status::KILLED  },
        { "running", Sprite_Status::RUNNING },
        { "stopped", Sprite_Status::STOPPED }
    };

    return string_map[status];
}

void Sprite::set_sprite_status(std::string _sprite_status) {
        sprite_status = string_to_status(_sprite_status);
        status_text->set_text(_sprite_status);
}

 void Sprite::set_focus(bool is_focus) {
    LOG(INFO) << "trying to set focus to "<< is_focus;
    focus_icon->set_renderable(is_focus);
 }

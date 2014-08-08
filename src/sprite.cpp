#include "image.hpp"
#include "engine.hpp"
#include "entitythread.hpp"
#include "map_viewer.hpp"
#include "renderable_component.hpp"
#include "sprite.hpp"

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

Sprite::Sprite(glm::ivec2 position, std::string name, int sheet_id, std::string sheet_name):
    MapObject(position, name, sheet_id, sheet_name) {

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
        blocked_tiles.insert(std::make_pair("stood on", Engine::get_map_viewer()->get_map()->block_tile(position)));
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

// TODO: reimplement for changing position

// void Sprite::set_y_position(int y_pos) {
//     for (auto item: get_inventory()) {
//         item->set_x_position(x_position);
//         item->set_y_position(y_position);
//     }
//     y_position = y_pos;
// }
// void Sprite::set_x_position(int x_pos) {
//     for (auto item: get_inventory()) {
//         item->set_x_position(x_position);
//         item->set_y_position(y_position);
//     }
//     x_position = x_pos;
// }
//
// void Sprite::set_y_position(double y_pos) {
//     for (auto item: get_inventory()) {
//         item->set_x_position(x_position);
//         item->set_y_position(y_position);
//     }
//     y_position = y_pos;
// }
// void Sprite::set_x_position(double x_pos) {
//     for (auto item: get_inventory()) {
//         item->set_x_position(x_position);
//         item->set_y_position(y_position);
//     }
//     x_position = x_pos;
// }

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


Status Sprite::string_to_status(std::string status) {
    std::map<std::string,Status> string_map;
    string_map["running"] = RUNNING;
    string_map["stopped"] = STOPPED;
    string_map[""] = NOTHING;
    string_map["failed"] = FAILED;
    string_map["killed"] = KILLED;
    return string_map[status];
}

void Sprite::set_sprite_status(std::string _sprite_status) {
        sprite_status = string_to_status(_sprite_status);
        status_text->set_text(_sprite_status);
}

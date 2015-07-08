#include <glog/logging.h>
#include <string>

#include "object.hpp"
#include "entitythread.hpp"
#include "object_manager.hpp"
#include "walkability.hpp"

//Object::Object(): Object("") {}

/*Object::Object(std::string name): name(name) { TODO: Remove
    // Get a new id for the object
    ObjectManager& object_manager = ObjectManager::get_instance();

    // WTF: Why does a getter explicitly mutate
    object_manager.get_next_id(this);

    // TODO: Maybe add the object to the object manager here
}*/

Object::Object(glm::vec2 position,
                     std::string name,
                     Walkability walkability,
                     AnimationFrames frames,
                     std::string start_frame):
                     Object(name),
                     walkability(walkability),
                     position(position),
                     frames(frames)
    {

        VLOG(2) << "New map object: " << name;

        regenerate_blockers();

        init_shaders();
        // Hack hack hack
        load_textures(frames.get_frame(start_frame));
        generate_tex_data(frames.get_frame(start_frame));
        generate_vertex_data();

        LOG(INFO) << "MapObject initialized";
}

Object::~Object() {
    LOG(INFO) << "OBJECT DESTROYING (" << id << ")  " << name << std::endl;
}

void Object::set_id(int new_id) {
    id = new_id;
}

void Object::set_name(std::string new_name) {
    name = new_name;
}

void Object::set_position(glm::vec2 position) {
    this->position = position;
    VLOG(2) << std::fixed << position.x << " " << position.y;
    regenerate_blockers();
}

void Object::regenerate_blockers() {
    body_blockers.clear();
    switch (walkability) {
        case Walkability::WALKABLE: {
            break;
        }

        case Walkability::BLOCKED: {
            int x_left(int(position.x));
            int y_bottom(int(position.y));
            VLOG(2) << std::fixed << position.y << " " << position.x;
            // If non-integral, the left or top have a higher
            // tile number. If integral, they do not.
            //
            // The test is done by checking if the truncation
            // changed the value
            int x_right(x_left   + (float(x_left)   != position.x));
            int y_top  (y_bottom + (float(y_bottom) != position.y));

            auto *map = Engine::get_map_viewer()->get_map();
            body_blockers = {
                map->block_tile(glm::ivec2(x_left,  y_top)),
                map->block_tile(glm::ivec2(x_left,  y_bottom)),
                map->block_tile(glm::ivec2(x_right, y_top)),
                map->block_tile(glm::ivec2(x_right, y_bottom))
            };

            break;
        }

        default: {
            throw std::runtime_error("WTF do I do now?");
        }
    }
}

void Object::set_state_on_moving_start(glm::ivec2) {
    moving = true;
}

void MapObject::set_state_on_moving_finish() {
    moving = false;

    // Remove all elements from container including and following
    // any prior occurence of this position, to remove redundant loops.
    while (positions.find(position) != std::end(positions)) {
        positions.get<insertion_order>().pop_back();
    }

    // Insert the position as most recent position
    positions.insert(position);
}

void Object::set_tile(std::pair<int, std::string> tile) {
    load_textures(tile);
    generate_tex_data(tile);
}

void Object::set_object_status(std::string _object_status) {
    object_status = string_to_status(_object_status);

    auto status_icon = ObjectManager::get_instance().get_object<Object>(status_icon_id);
    if (!status_icon) {
        LOG(ERROR) << "Object manager no longer has status_icon";
        return;
    }

    switch (object_status) {
        case Sprite_Status::NOTHING:
        case Sprite_Status::STOPPED:
            status_icon->set_tile(TextureAtlas::from_name("gui/status/stationary"));
            break;

        case Sprite_Status::KILLED:
            status_icon->set_tile(TextureAtlas::from_name("gui/status/failed"));
            break;

        case Sprite_Status::RUNNING:
            status_icon->set_tile(TextureAtlas::from_name("gui/status/running"));
            break;

        case Sprite_Status::FAILED:
            // TODO: stopping should also be here
            status_icon->set_tile(TextureAtlas::from_name("gui/status/failed"));
            break;
    }
}

void Object::set_focus(bool _is_focus) {
    // check focus is actually being changed
    if (_is_focus != is_focus) {
        LOG(INFO) << "trying to set focus to "<< is_focus;
        is_focus = _is_focus;

        auto focus_icon = ObjectManager::get_instance().get_object<MapObject>(focus_icon_id);
        if (!focus_icon) {
            LOG(ERROR) << "Object manager no longer has focus_icon";
            return;
        }

        focus_icon->set_renderable(is_focus);
    }
}


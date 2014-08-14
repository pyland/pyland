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
#include "texture_atlas.hpp"
#include "walkability.hpp"

Sprite::Sprite(glm::ivec2 position,
               std::string name,
               Walkability walkability,
               int sheet_id,
               std::string sheet_name):

    MapObject(position, name, walkability, sheet_id, sheet_name) {
        auto map_viewer(Engine::get_map_viewer());

        // Setting up sprite text
        TextFont myfont = Engine::get_game_font();

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

void Sprite::generate_tex_data() {
  
    //holds the texture data
    //need 12 float for the 2D texture coordinates
    int num_dimensions = 2;
    int num_floats_per_tile = num_dimensions*6; //GLTRIANGLES so need 6 vertices
    int num_floats = num_floats_per_tile;

    //Add coordinates for overlays and underlays
    num_floats += int(overlay_ids.size())*num_floats_per_tile;
    num_floats += int(underlay_ids.size())*num_floats_per_tile;

    GLfloat* sprite_tex_data = nullptr;
    try {
        sprite_tex_data = new GLfloat[sizeof(GLfloat)*num_floats];
    }
    catch(std::bad_alloc& ba) {
        LOG(ERROR) << "ERROR in Sprite::generate_tex_data(), cannot allocate memory";
        return;
    }

    //offset into texture buffer
    int offset = 0;

    //Underlays
    for(int underlay_id : underlay_ids) {
        std::tuple<float,float,float,float> bounds = renderable_component.get_texture()->index_to_coords(underlay_id);

        //bottom left
        sprite_tex_data[offset + 0]  = std::get<0>(bounds);
        sprite_tex_data[offset + 1]  = std::get<2>(bounds);

        //top left
        sprite_tex_data[offset + 2]  = std::get<0>(bounds);
        sprite_tex_data[offset + 3]  = std::get<3>(bounds);

        //bottom right
        sprite_tex_data[offset + 4]  = std::get<1>(bounds);
        sprite_tex_data[offset + 5]  = std::get<2>(bounds);

        //top left
        sprite_tex_data[offset + 6]  = std::get<0>(bounds);
        sprite_tex_data[offset + 7]  = std::get<3>(bounds);

        //top right
        sprite_tex_data[offset + 8]  = std::get<1>(bounds);
        sprite_tex_data[offset + 9]  = std::get<3>(bounds);

        //bottom right
        sprite_tex_data[offset + 10] = std::get<1>(bounds);
        sprite_tex_data[offset + 11] = std::get<2>(bounds);


        offset+= num_floats_per_tile;
    }

    std::tuple<float,float,float,float> bounds = renderable_component.get_texture()->index_to_coords(sheet_id);

    //bottom left
    sprite_tex_data[offset + 0]  = std::get<0>(bounds);
    sprite_tex_data[offset + 1]  = std::get<2>(bounds);

    //top left
    sprite_tex_data[offset + 2]  = std::get<0>(bounds);
    sprite_tex_data[offset + 3]  = std::get<3>(bounds);

    //bottom right
    sprite_tex_data[offset + 4]  = std::get<1>(bounds);
    sprite_tex_data[offset + 5]  = std::get<2>(bounds);

    //top left
    sprite_tex_data[offset + 6]  = std::get<0>(bounds);
    sprite_tex_data[offset + 7]  = std::get<3>(bounds);

    //top right
    sprite_tex_data[offset + 8]  = std::get<1>(bounds);
    sprite_tex_data[offset + 9]  = std::get<3>(bounds);

    //bottom right
    sprite_tex_data[offset + 10] = std::get<1>(bounds);
    sprite_tex_data[offset + 11] = std::get<2>(bounds);

    offset += num_floats_per_tile;

    //Overlays

    for(int overlay_id : overlay_ids) {

        std::tuple<float,float,float,float> bounds = renderable_component.get_texture()->index_to_coords(overlay_id);

        //bottom left
        sprite_tex_data[offset + 0]  = std::get<0>(bounds);
        sprite_tex_data[offset + 1]  = std::get<2>(bounds);

        //top left
        sprite_tex_data[offset + 2]  = std::get<0>(bounds);
        sprite_tex_data[offset + 3]  = std::get<3>(bounds);

        //bottom right
        sprite_tex_data[offset + 4]  = std::get<1>(bounds);
        sprite_tex_data[offset + 5]  = std::get<2>(bounds);

        //top left
        sprite_tex_data[offset + 6]  = std::get<0>(bounds);
        sprite_tex_data[offset + 7]  = std::get<3>(bounds);

        //top right
        sprite_tex_data[offset + 8]  = std::get<1>(bounds);
        sprite_tex_data[offset + 9]  = std::get<3>(bounds);

        //bottom right
        sprite_tex_data[offset + 10] = std::get<1>(bounds);
        sprite_tex_data[offset + 11] = std::get<2>(bounds);

        offset+= num_floats_per_tile;
    }


    renderable_component.set_texture_coords_data(sprite_tex_data, sizeof(GLfloat)*num_floats, false);
} 

void Sprite::generate_vertex_data() {
    //holds the sprite vertex data
    int num_dimensions = 2;
    int num_floats_per_tile = num_dimensions*6; //GLTRIANGLES so need 6 vertices

    int num_floats = num_floats_per_tile;

    //Add coordinates for overlays and underlays
    num_floats += int(overlay_ids.size())*num_floats_per_tile;
    num_floats += int(underlay_ids.size())*num_floats_per_tile;

    GLfloat* sprite_data = nullptr;
    try {

        sprite_data = new GLfloat[sizeof(GLfloat)*num_floats];
    }
    catch(std::bad_alloc& ba) {
        LOG(ERROR) << "ERROR in Sprite::generate_vertex_data(), cannot allocate memory";
        return;
    }

    float scale =  float(Engine::get_tile_size()) * Engine::get_global_scale();

    //offset into texture buffer
    int offset = 0;
    int index = 0;

    //Underlays
    for(int i = 0; i < int(underlay_ids.size()); i++) {
        GLfloat x_offset = (GLfloat)underlay_offsets[index].first;
        GLfloat y_offset = (GLfloat)underlay_offsets[index].second;
        GLfloat width = (GLfloat)underlay_dimensions[index].first;
        GLfloat height = (GLfloat)underlay_dimensions[index].second;

        //bottom left 
        sprite_data[offset + 0] = x_offset*scale;
        sprite_data[offset + 1] = y_offset*scale;

        //top left
        sprite_data[offset + 2] = x_offset*scale;
        sprite_data[offset + 3] = y_offset*scale + scale * height;

        //bottom right
        sprite_data[offset + 4] = x_offset*scale + scale * width;
        sprite_data[offset + 5] = y_offset*scale;

        //top left
        sprite_data[offset + 6] = x_offset*scale;
        sprite_data[offset + 7] = y_offset*scale + scale * height;

        //top right
        sprite_data[offset + 8] = x_offset*scale + scale * width;
        sprite_data[offset + 9] = y_offset*scale + scale * height;

        //bottom right
        sprite_data[offset + 10] = x_offset*scale + scale * width;
        sprite_data[offset + 11] = y_offset*scale;

        offset+= num_floats_per_tile;
        index++;
    }
    //bottom left 
    sprite_data[offset + 0] = 0;
    sprite_data[offset + 1] = 0;

    //top left
    sprite_data[offset + 2] = 0;
    sprite_data[offset + 3] = scale;

    //bottom right
    sprite_data[offset + 4] = scale;
    sprite_data[offset + 5] = 0;

    //top left
    sprite_data[offset + 6] = 0;
    sprite_data[offset + 7] = scale;

    //top right
    sprite_data[offset + 8] = scale;
    sprite_data[offset + 9] = scale;

    //bottom right
    sprite_data[offset + 10] = scale;
    sprite_data[offset + 11] = 0;

    offset += num_floats_per_tile;

    index = 0;
    //Overlays
    for(int i = 0; int(overlay_ids.size()); i++) {
        GLfloat x_offset = (GLfloat)overlay_offsets[index].first;
        GLfloat y_offset = (GLfloat)overlay_offsets[index].second;
        GLfloat width = (GLfloat)overlay_dimensions[index].first;
        GLfloat height = (GLfloat)overlay_dimensions[index].second;

        //bottom left 
        sprite_data[offset + 0] = x_offset*scale;
        sprite_data[offset + 1] = y_offset*scale;

        //top left
        sprite_data[offset + 2] = x_offset*scale;
        sprite_data[offset + 3] = y_offset*scale + scale * height;

        //bottom right
        sprite_data[offset + 4] = x_offset*scale + scale * width;
        sprite_data[offset + 5] = y_offset*scale;

        //top left
        sprite_data[offset + 6] = x_offset*scale;
        sprite_data[offset + 7] = y_offset*scale + scale * height;

        //top right
        sprite_data[offset + 8] = x_offset*scale + scale * width;
        sprite_data[offset + 9] = y_offset*scale + scale * height;

        //bottom right
        sprite_data[offset + 10] = x_offset*scale + scale * width;
        sprite_data[offset + 11] = y_offset*scale;

        offset+= num_floats_per_tile;
        index++;
    }


    renderable_component.set_vertex_data(sprite_data,sizeof(GLfloat)*num_floats, false);
    renderable_component.set_num_vertices_render(num_floats/num_dimensions);//GL_TRIANGLES being used
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

void Sprite::set_focus(bool _is_focus) {
    // check focus is actually being changed
    if (_is_focus != is_focus) {
        LOG(INFO) << "trying to set focus to "<< is_focus;
        is_focus = _is_focus;
        focus_icon->set_renderable(is_focus);
    }
}

void Sprite::add_overlay(int overlay_id, float width, float height, float x_offset, float y_offset) {
    overlay_ids.push_back(overlay_id);
    std::pair<float, float> dimensions = std::make_pair(width,  height);
    overlay_dimensions.push_back(dimensions);
    std::pair<float, float> offsets = std::make_pair(x_offset, y_offset);
    overlay_offsets.push_back(offsets);
    generate_tex_data();
}

void Sprite::remove_overlay(int overlay_id) {

    //Index to delete at
    int index = 0;

    //Remove it from the vector
    for(auto iter = overlay_ids.begin(); iter != overlay_ids.end(); ++iter) {
        if(overlay_id == *iter) {
            overlay_ids.erase(iter);
            break;
        }
        index++;
    }
    overlay_dimensions.erase(overlay_dimensions.begin()+index);
    overlay_offsets.erase(overlay_offsets.begin()+index);
    generate_tex_data();
}

void Sprite::add_underlay(int underlay_id, float width, float height, float x_offset, float y_offset) {
    underlay_ids.push_back(underlay_id);
    std::pair<float, float> dimensions = std::make_pair(width,  height);
    underlay_dimensions.push_back(dimensions);
    std::pair<float,float> offsets = std::make_pair(x_offset, y_offset);
    underlay_offsets.push_back(offsets);
    generate_tex_data();
}

void Sprite::remove_underlay(int underlay_id)  {
    //Index to delete at
    int index = 0;

    //Remove it from the vector
    for(auto iter = underlay_ids.begin(); iter != underlay_ids.end(); ++iter) {
        if(underlay_id == *iter) {
            underlay_ids.erase(iter);
            break;
        }
        index++;
    }
    underlay_dimensions.erase(underlay_dimensions.begin()+index);
    underlay_offsets.erase(underlay_offsets.begin()+index);
    generate_tex_data();
}

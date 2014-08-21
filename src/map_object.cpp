#include <exception>
#include <fstream>
#include <glog/logging.h>
#include <ios>
#include <memory>
#include <new>
#include <stdexcept>
#include <tuple>

#include "animation_frames.hpp"
#include "cacheable_resource.hpp"
#include "engine.hpp"
#include "map_object.hpp"
#include "map_viewer.hpp"
#include "shader.hpp"
#include "texture_atlas.hpp"
#include "walkability.hpp"

#ifdef USE_GLES
#include <GLES2/gl2.h>
#endif

#ifdef USE_GL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif

// WTF
MapObject::MapObject(glm::vec2 position,
                     std::string name,
                     Walkability walkability,
                     AnimationFrames frames,
                     std::string start_frame):
    Object(name),
    render_above_sprite(false),
    walkability(walkability),
    position(position),
    frames(frames) {

        VLOG(2) << "New map object: " << name;

        regenerate_blockers();

        init_shaders();
        // Hack hack hack
        load_textures(frames.get_frame(start_frame));
        generate_tex_data(frames.get_frame(start_frame));
        generate_vertex_data();

        LOG(INFO) << "MapObject initialized";
}

MapObject::~MapObject() {
    LOG(INFO) << "MapObject destructed";
}

void MapObject::set_walkability(Walkability walkability) {
    this->walkability = walkability;
    regenerate_blockers();
}

void MapObject::regenerate_blockers() {
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

void MapObject::generate_tex_data(std::pair<int, std::string> tile) {
    // holds the texture data
    // need 12 float for the 2D texture coordinates
    int num_floats = 12;

    GLfloat *map_object_tex_data;
    try {
        map_object_tex_data = new GLfloat[sizeof(GLfloat)*num_floats];
    }
    catch(std::bad_alloc &) {
        LOG(ERROR) << "ERROR in MapObject::generate_tex_data(), cannot allocate memory";
        return;
    }

    std::tuple<float,float,float,float> bounds(
        renderable_component.get_texture()->index_to_coords(tile.first)
    );

    // bottom left
    map_object_tex_data[ 0] = std::get<0>(bounds);
    map_object_tex_data[ 1] = std::get<2>(bounds);

    // top left
    map_object_tex_data[ 2] = std::get<0>(bounds);
    map_object_tex_data[ 3] = std::get<3>(bounds);

    // bottom right
    map_object_tex_data[ 4] = std::get<1>(bounds);
    map_object_tex_data[ 5] = std::get<2>(bounds);

    // top left
    map_object_tex_data[ 6] = std::get<0>(bounds);
    map_object_tex_data[ 7] = std::get<3>(bounds);

    // top right
    map_object_tex_data[ 8] = std::get<1>(bounds);
    map_object_tex_data[ 9] = std::get<3>(bounds);

    // bottom right
    map_object_tex_data[10] = std::get<1>(bounds);
    map_object_tex_data[11] = std::get<2>(bounds);

    renderable_component.set_texture_coords_data(map_object_tex_data, sizeof(GLfloat)*num_floats, false);
}

void MapObject::set_position(glm::vec2 position) {
    this->position = position; 
    VLOG(2) << std::fixed << position.x << " " << position.y;
    regenerate_blockers();
}

void MapObject::set_tile(std::pair<int, std::string> tile) {
    load_textures(tile);
    generate_tex_data(tile);
}

void MapObject::generate_vertex_data() {
    //holds the map object's vertex data
    int num_dimensions(2);
    int num_floats(num_dimensions * 6); // GL's Triangles
    GLfloat *map_object_vert_data(nullptr);

    try {
        map_object_vert_data = new GLfloat[sizeof(GLfloat)*num_floats];
    }
    catch(std::bad_alloc& ba) {
        LOG(ERROR) << "ERROR in MapObject::generate_vertex_data(), cannot allocate memory";
        return;
    }

    //bottom left
    map_object_vert_data[0]  = 0;
    map_object_vert_data[1]  = 0;

    //top left
    map_object_vert_data[2]  = 0;
    map_object_vert_data[3]  = 1;

    //bottom right
    map_object_vert_data[4]  = 1;
    map_object_vert_data[5]  = 0;

    //top left
    map_object_vert_data[6]  = 0;
    map_object_vert_data[7]  = 1;

    //top right
    map_object_vert_data[8]  = 1;
    map_object_vert_data[9]  = 1;

    //bottom right
    map_object_vert_data[10] = 1;
    map_object_vert_data[11] = 0;

    renderable_component.set_vertex_data(map_object_vert_data, sizeof(GLfloat)*num_floats, false);
    renderable_component.set_num_vertices_render(num_floats / num_dimensions);//GL_TRIANGLES being used
}

void MapObject::set_state_on_moving_start(glm::ivec2) {
    moving = true;
}

void MapObject::set_state_on_moving_finish() {
    moving = false;
}

// TODO: rewrite
void MapObject::load_textures(std::pair<int, std::string> tile) {
    renderable_component.set_texture(TextureAtlas::get_shared(tile.second));
}

bool MapObject::init_shaders() {
    std::shared_ptr<Shader> shader;
    try {
        shader = Shader::get_shared("tile_shader");
    }
    catch (std::exception e) {
        LOG(ERROR) << "Failed to create the shader";
        return false;
    }

    //Set the shader
    renderable_component.set_shader(shader);
    return true;
}

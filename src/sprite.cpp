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
Sprite::Sprite(glm::ivec2 position, std::string name): MapObject(position, name) {
    // TODO: do this properly
    status_text->set_text("");
    LOG(INFO) << "Sprite initialized";
}

Sprite::~Sprite() {
    LOG(INFO) << "Sprite destructed";
}


void Sprite::set_state_on_moving_start(glm::ivec2 target) {
    moving = true;
    blocked_tiles.insert(std::make_pair("walking to", Engine::get_map_viewer()->get_map()->block_tile(target)));
}

void Sprite::set_state_on_moving_finish() {
    moving = false;
    blocked_tiles.erase("stood on");
    blocked_tiles.insert(std::make_pair("stood on", blocked_tiles.at("walking to")));
    blocked_tiles.erase("walking to");
}


void Sprite::generate_tex_data() {
    //holds the texture data
    //need 12 float for the 2D texture coordinates
    int num_floats = 12;

    GLfloat* sprite_tex_data = nullptr;
    try {

        sprite_tex_data = new GLfloat[sizeof(GLfloat)*num_floats];
    }
    catch(std::bad_alloc& ba) {
        LOG(ERROR) << "ERROR in Sprite::generate_tex_data(), cannot allocate memory";
        return;
    }

    GLfloat offset_x = GLfloat(Engine::get_tile_size()) / (GLfloat)renderable_component.get_texture_image()->store_width;
    GLfloat offset_y = GLfloat(Engine::get_tile_size()) / (GLfloat)renderable_component.get_texture_image()->store_height;

    //bottom left
    sprite_tex_data[0]  = offset_x * GLfloat(4.0);
    sprite_tex_data[1]  = offset_y * GLfloat(7.0);

    //top left
    sprite_tex_data[2]  = offset_x * GLfloat(4.0);
    sprite_tex_data[3]  = offset_y * GLfloat(8.0);

    //bottom right
    sprite_tex_data[4]  = offset_x * GLfloat(5.0);
    sprite_tex_data[5]  = offset_y * GLfloat(7.0);

    //top left
    sprite_tex_data[6]  = offset_x * GLfloat(4.0);
    sprite_tex_data[7]  = offset_y * GLfloat(8.0);

    //top right
    sprite_tex_data[8]  = offset_x * GLfloat(5.0);
    sprite_tex_data[9]  = offset_y * GLfloat(8.0);

    //bottom right
    sprite_tex_data[10] = offset_x * GLfloat(5.0);
    sprite_tex_data[11] = offset_y * GLfloat(7.0);

    renderable_component.set_texture_coords_data(sprite_tex_data, sizeof(GLfloat)*num_floats, false);
}

void Sprite::generate_vertex_data() {
    //holds the sprite vertex data
    int num_dimensions = 2;
    int num_floats = num_dimensions*6;//GLTRIANGLES
    GLfloat* sprite_data = nullptr;
    try {

        sprite_data = new GLfloat[sizeof(GLfloat)*num_floats];
    }
    catch(std::bad_alloc& ba) {
        LOG(ERROR) << "ERROR in Sprite::generate_vertex_data(), cannot allocate memory";
        return;
    }

    float scale =(float)( Engine::get_tile_size() * Engine::get_global_scale());

    //bottom left
    sprite_data[0] = 0;
    sprite_data[1] = 0;

    //top left
    sprite_data[2] = 0;
    sprite_data[3] = scale;

    //bottom right
    sprite_data[4] = scale;
    sprite_data[5] = 0;

    //top left
    sprite_data[6] = 0;
    sprite_data[7] = scale;

    //top right
    sprite_data[8] = scale;
    sprite_data[9] = scale;

    //bottom right
    sprite_data[10] = scale;
    sprite_data[11] = 0;

    renderable_component.set_vertex_data(sprite_data,sizeof(GLfloat)*num_floats, false);
    renderable_component.set_num_vertices_render(num_floats/num_dimensions);//GL_TRIANGLES being used
}

// TODO: Fix...
void Sprite::load_textures() {
    Image* texture_image = nullptr;

    try {
        texture_image = new Image("../resources/characters_1.png");
    }
    catch(std::exception e) {
        texture_image = nullptr;
        LOG(ERROR) << "Failed to create texture";
        return;
    }

    //Set the texture data in the rederable component
    renderable_component.set_texture_image(texture_image);
}

bool Sprite::init_shaders() {
    std::shared_ptr<Shader> shader;
    try {
        shader = Shader::get_shared_shader("tile_shader");
    }
    catch (std::exception e) {
        LOG(ERROR) << "Failed to create the shader";
        return false;
    }

    //Set the shader
    renderable_component.set_shader(shader);

    return true;

}

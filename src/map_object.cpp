#include <fstream>
#include <glog/logging.h>
#include <glm/vec2.hpp>
#include <iostream>
#include <new>

#include "engine.hpp"
#include "entitythread.hpp"
#include "image.hpp"
#include "map_object.hpp"
#include "map_viewer.hpp"
#include "renderable_component.hpp"

#ifdef USE_GLES
#include <GLES2/gl2.h>
#endif

#ifdef USE_GL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif

// WTF
MapObject::MapObject() {
    LOG(INFO) << "Empty MapObject created";
}

// WTF
MapObject::MapObject(glm::vec2 position, std::string name): position(position)  {
    VLOG(2) << "New map object: " << name;

    set_name(name);

    init_shaders();
    load_textures();
    generate_tex_data();
    generate_vertex_data();

    auto map_viewer(Engine::get_map_viewer());

    // Setting up sprite text
    Typeface mytype("../fonts/hans-kendrick/HansKendrick-Regular.ttf");
    TextFont myfont(mytype, 18);

    // TODO: Smart pointer
    object_text = new Text(Engine::get_map_viewer()->get_window(), myfont, true);
    object_text->set_text(name);
    glm::ivec2 pixel_position(map_viewer->tile_to_pixel(position));
    object_text->move(pixel_position.x, pixel_position.y);
    object_text->resize(100,100);
    LOG(INFO) << "Setting up text at " << pixel_position.x << ", " << pixel_position.y;

    // setting up status text
    status_text = new Text(map_viewer->get_window(), myfont, true);
    status_text->set_text("awaiting...");
    glm::ivec2 pixel_text(map_viewer->tile_to_pixel(position));
    status_text->move(pixel_text.x, pixel_text.y + 100);
    status_text->resize(100,100);

    // TODO: Starting positions should be integral as of currently. Check or fix.
    //
    // Make a map object blocked
    // In future this might not be needed
    blocked_tiles.insert(std::make_pair("stood on", map_viewer->get_map()->block_tile(position)));

    LOG(INFO) << "MapObject initialized";
}

MapObject::~MapObject() {
    delete object_text;
    delete status_text;
    LOG(INFO) << "MapObject destructed";
}


void MapObject::set_state_on_moving_start(glm::ivec2 target) {
    moving = true;
    blocked_tiles.insert(std::make_pair("walking to", Engine::get_map_viewer()->get_map()->block_tile(target)));
}

void MapObject::set_state_on_moving_finish() {
    moving = false;
    blocked_tiles.erase("stood on");
    blocked_tiles.insert(std::make_pair("stood on", blocked_tiles.at("walking to")));
    blocked_tiles.erase("walking to");
}


void MapObject::generate_tex_data() {
  
    //holds the texture data
    //need 12 float for the 2D texture coordinates
    int num_floats = 12;

    GLfloat* map_object_tex_data = nullptr;
    try {

        map_object_tex_data = new GLfloat[sizeof(GLfloat)*num_floats];
    }
    catch(std::bad_alloc& ba) {
        LOG(ERROR) << "ERROR in MapObject::generate_tex_data(), cannot allocate memory";
        return;
    }

    GLfloat offset_x = GLfloat(Engine::get_tile_size()) / (GLfloat)renderable_component.get_texture_image()->store_width;
    GLfloat offset_y = GLfloat(Engine::get_tile_size()) / (GLfloat)renderable_component.get_texture_image()->store_height;

    //bottom left
    map_object_tex_data[0]  = offset_x * GLfloat(4.0);
    map_object_tex_data[1]  = offset_y * GLfloat(7.0);

    //top left
    map_object_tex_data[2]  = offset_x * GLfloat(4.0);
    map_object_tex_data[3]  = offset_y * GLfloat(8.0); 

    //bottom right
    map_object_tex_data[4]  = offset_x * GLfloat(5.0);
    map_object_tex_data[5]  = offset_y * GLfloat(7.0);

    //top left
    map_object_tex_data[6]  = offset_x * GLfloat(4.0);
    map_object_tex_data[7]  = offset_y * GLfloat(8.0);

    //top right
    map_object_tex_data[8]  = offset_x * GLfloat(5.0);
    map_object_tex_data[9]  = offset_y * GLfloat(8.0);

    //bottom right
    map_object_tex_data[10] = offset_x * GLfloat(5.0);
    map_object_tex_data[11] = offset_y * GLfloat(7.0);

    renderable_component.set_texture_coords_data(map_object_tex_data, sizeof(GLfloat)*num_floats, false);
} 

void MapObject::generate_vertex_data() {
    //holds the map object's vertex data
    int num_dimensions = 2;
    int num_floats = num_dimensions*6;//GLTRIANGLES
    GLfloat* map_object_vert_data = nullptr;
    try {

        map_object_vert_data = new GLfloat[sizeof(GLfloat)*num_floats];
    }
    catch(std::bad_alloc& ba) {
        LOG(ERROR) << "ERROR in MapObject::generate_vertex_data(), cannot allocate memory";
        return;
    }

    float scale =(float)( Engine::get_tile_size() * Engine::get_global_scale());
 
    //bottom left 
    map_object_vert_data[0] = 0;
    map_object_vert_data[1] = 0;

    //top left
    map_object_vert_data[2] = 0;
    map_object_vert_data[3] = scale;

    //bottom right
    map_object_vert_data[4] = scale;
    map_object_vert_data[5] = 0;

    //top left
    map_object_vert_data[6] = 0;
    map_object_vert_data[7] = scale;

    //top right
    map_object_vert_data[8] = scale;
    map_object_vert_data[9] = scale;

    //bottom right
    map_object_vert_data[10] = scale;
    map_object_vert_data[11] = 0;

    renderable_component.set_vertex_data(map_object_vert_data,sizeof(GLfloat)*num_floats, false);
    renderable_component.set_num_vertices_render(num_floats/num_dimensions);//GL_TRIANGLES being used
}

// TODO: rewrite
void MapObject::load_textures() {
    Image* texture_image = nullptr;

    try {
        texture_image = new Image("../resources/characters_1.png");
    }
    catch(std::exception e) {
        delete texture_image;
        texture_image = nullptr;
        LOG(ERROR) << "Failed to create texture";
        return;
    }

    //Set the texture data in the rederable component
    renderable_component.set_texture_image(texture_image);
}

bool MapObject::init_shaders() {
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


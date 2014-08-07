#include "map_object.hpp"
#include "image.hpp"
#include "engine_api.hpp"
#include "entitythread.hpp"
#include "map_viewer.hpp"
#include "renderable_component.hpp"

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
MapObject::MapObject() {
    LOG(INFO) << "register new map object";
    x_position = 0;
    y_position = 0;
    name = "";
    tile_sheet_id = 0;
    tile_sheet = "../resources/basictiles_2.png";

    init_shaders();
    load_textures();
    generate_tex_data();
    generate_vertex_data();

    // Starting positions should be integral
    assert(trunc(x_position) == x_position);
    assert(trunc(y_position) == y_position);

    //Make a map object blocked
    blocked_tiles.insert(std::make_pair("stood on", Engine::get_map_viewer()->get_map()->block_tile(
        Vec2D(int(x_position), int(y_position))
    )));


    LOG(INFO) << "MapObject initialized";

}

MapObject::MapObject(int _x_position, int _y_position, std::string _name, int _tile_sheet_id, std::string _tile_sheet) {
    LOG(INFO) << "register new map object called " << _name;
    x_position = _x_position;
    y_position = _y_position;
    name = _name;
    tile_sheet_id = _tile_sheet_id;
    tile_sheet = _tile_sheet;

    init_shaders();
    load_textures();
    generate_tex_data();
    generate_vertex_data();

    // Starting positions should be integral
    assert(trunc(x_position) == x_position);
    assert(trunc(y_position) == y_position);

    //Make a map object blocked
    if ( get_walkability() == Walkability::BLOCKED) {
        blocked_tiles.insert(std::make_pair("stood on", Engine::get_map_viewer()->get_map()->block_tile(
            Vec2D(int(x_position), int(y_position))
        )));
    }


    LOG(INFO) << "MapObject initialized";

}

MapObject::~MapObject() {
    LOG(INFO) << "MapObject destructed";
}


void MapObject::set_state_on_moving_start(Vec2D target) {
    moving = true;
    // TODO: fix walkability if this changes, currently we assume set_walkability 
    // it not used during movement
    if ( get_walkability() == Walkability::BLOCKED) {
        blocked_tiles.insert(std::make_pair("walking to", Engine::get_map_viewer()->get_map()->block_tile(target)));
    }
}

void MapObject::set_state_on_moving_finish() {
    moving = false;
    if ( get_walkability() == Walkability::BLOCKED) {
        blocked_tiles.erase("stood on");
        blocked_tiles.insert(std::make_pair("stood on", blocked_tiles.at("walking to")));
        blocked_tiles.erase("walking to");
    }
}


void MapObject::generate_tex_data() {
  
    //holds the texture data
    //need 12 float for the 2D texture coordinates
    int num_floats = 12;
    std::cout << " CALLEDD " << std::endl;
    GLfloat* map_object_tex_data = nullptr;
    try {

        map_object_tex_data = new GLfloat[sizeof(GLfloat)*num_floats];
    }
    catch(std::bad_alloc& ba) {
        LOG(ERROR) << "ERROR in MapObject::generate_tex_data(), cannot allocate memory";
        return;
    }

    int image_width = renderable_component.get_texture_image()->store_width;
    int image_height = renderable_component.get_texture_image()->store_height;
    if(image_width == 0 || image_height == 0 ) {
        LOG(ERROR) << "At least one image dimension is 0";
        return;
    }

    GLfloat inc_x = GLfloat(Engine::get_tile_size()) / (GLfloat)image_width;
    GLfloat inc_y = GLfloat(Engine::get_tile_size()) / (GLfloat)image_height;
    if(inc_x == 0.0f  ||inc_y == 0.0f) {
        LOG(ERROR) << "Increments are 0";
        return;
    }

    //Tile ids are from top left but opengl texture coordinates are bottom left so adjust as needed
    GLfloat offset_x = (GLfloat)(tile_sheet_id % (int)(image_width/Engine::get_tile_size()))*inc_x;
    GLfloat offset_y = (1.0f - inc_y) -  (GLfloat)(tile_sheet_id / (int)(image_width/Engine::get_tile_size()))*inc_y;

    std::cout << " ID " << tile_sheet_id << " WIDTH " << image_width << " INC " << inc_x << std::endl;    
    std::cout <<" SETTING" << (int)(image_width/inc_x) << std::endl;

    //bottom left
    map_object_tex_data[0]  = offset_x;
    map_object_tex_data[1]  = offset_y;

    //top left
    map_object_tex_data[2]  = offset_x;
    map_object_tex_data[3]  = offset_y + inc_y;

    //bottom right
    map_object_tex_data[4]  = offset_x + inc_x;
    map_object_tex_data[5]  = offset_y;

    //top left
    map_object_tex_data[6]  = offset_x;
    map_object_tex_data[7]  = offset_y + inc_y;

    //top right
    map_object_tex_data[8]  = offset_x + inc_x;
    map_object_tex_data[9]  = offset_y + inc_y;

    //bottom right
    map_object_tex_data[10] = offset_x + inc_x;
    map_object_tex_data[11] = offset_y; 


    renderable_component.set_texture_coords_data(map_object_tex_data, sizeof(GLfloat)*num_floats, false);
} 
void MapObject::set_tile_sheet_id(int _tile_sheet_id) {
    tile_sheet_id  = _tile_sheet_id; 
    
    generate_tex_data();
}
void MapObject::set_tile_sheet(std::string _tile_sheet) {
    tile_sheet = _tile_sheet; 
    generate_tex_data();
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

void MapObject::load_textures() {
    Image* texture_image = nullptr;

    try {
        texture_image = new Image(tile_sheet.c_str());
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
}

 void MapObject::set_walkability(Walkability _walkability) { 
    walkability = _walkability;
    if ( walkability == Walkability::BLOCKED) {
        // don't need to check if was previously blocked or not
        blocked_tiles.insert(std::make_pair("stood on", Engine::get_map_viewer()->get_map()->block_tile(
            Vec2D(int(x_position), int(y_position))
        )));
    } else {
        blocked_tiles.erase("stood on");
    }
}


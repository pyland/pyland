#include <fstream>
#include <glog/logging.h>
#include <glm/vec2.hpp>
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
MapObject::MapObject(glm::vec2 position, std::string name, int sheet_id, std::string sheet_name):
    Object(name),
    sheet_name(sheet_name),
    sheet_id(sheet_id),
    position(position) {

        VLOG(2) << "New map object: " << name;

        init_shaders();
        load_textures();
        generate_tex_data();
        generate_vertex_data();

        LOG(INFO) << "MapObject initialized";
}

MapObject::~MapObject() {
    LOG(INFO) << "MapObject destructed";
}

void MapObject::generate_tex_data() {

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

    int image_width = renderable_component.get_texture_image()->store_width;
    int image_height = renderable_component.get_texture_image()->store_height;
    if (image_width == 0 || image_height == 0 ) {
        LOG(ERROR) << "At least one image dimension is 0";
        return;
    }

    GLfloat inc_x(Engine::get_tile_size() / GLfloat(image_width));
    GLfloat inc_y(Engine::get_tile_size() / GLfloat(image_height));
    if (inc_x == 0.0f || inc_y == 0.0f) {
        LOG(ERROR) << "Increments are 0";
        return;
    }

    // Tile ids are from top left but opengl texture coordinates are bottom left so adjust as needed
    GLfloat offset_x(               inc_x * GLfloat(sheet_id % int(image_width / Engine::get_tile_size())));
    GLfloat offset_y(1.0f - inc_y - inc_y * GLfloat(sheet_id / int(image_width / Engine::get_tile_size())));

    // bottom left
    map_object_tex_data[0]  = offset_x;
    map_object_tex_data[1]  = offset_y;

    // top left
    map_object_tex_data[2]  = offset_x;
    map_object_tex_data[3]  = offset_y + inc_y;

    // bottom right
    map_object_tex_data[4]  = offset_x + inc_x;
    map_object_tex_data[5]  = offset_y;

    // top left
    map_object_tex_data[6]  = offset_x;
    map_object_tex_data[7]  = offset_y + inc_y;

    // top right
    map_object_tex_data[8]  = offset_x + inc_x;
    map_object_tex_data[9]  = offset_y + inc_y;

    // bottom right
    map_object_tex_data[10] = offset_x + inc_x;
    map_object_tex_data[11] = offset_y;


    renderable_component.set_texture_coords_data(map_object_tex_data, sizeof(GLfloat)*num_floats, false);
}

void MapObject::set_sheet_id(int sheet_id) {
    this->sheet_id = sheet_id;
    generate_tex_data();
}

void MapObject::set_sheet_name(std::string sheet_name) {
    this->sheet_name = sheet_name;
    generate_tex_data();
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

    float scale(Engine::get_actual_tile_size());

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

    renderable_component.set_vertex_data(map_object_vert_data, sizeof(GLfloat)*num_floats, false);
    renderable_component.set_num_vertices_render(num_floats / num_dimensions);//GL_TRIANGLES being used
}

// TODO: rewrite
void MapObject::load_textures() {
    Image *texture_image = nullptr;

    try {
        texture_image = new Image(sheet_name.c_str());
    }
    catch (std::exception e) {
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

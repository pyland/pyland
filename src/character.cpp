#include "character.hpp"
#include "renderable_component.hpp"
#include "print_debug.hpp"
#include <iostream>
#include <fstream>


#ifdef USE_GLES

#include <GLES2/gl2.h>

#endif

#ifdef USE_GL

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#endif

#define TILESET_ELEMENT_SIZE 16
#define IMAGE2_SIZE_WIDTH 192
#define IMAGE2_SIZE_HEIGHT 128
#define GLOBAL_SCALE 2
#define IMAGE2_NUM_COMPONENTS 4




Character::Character() {
    init_shaders();
    generate_tex_data();
    generate_vertex_data();
    load_textures();
    std::cout << "CHARACTER CREATED" << std::endl;
}

Character::~Character() {
    delete []sprite_tex_data;
    delete []sprite_data;
    delete []tex_buf;

    std::cout << "DELETED CHARACTER" << std::endl;
}


void Character::generate_tex_data() {
  
    //holds the texture data
    //need 12 float for the 2D texture coordinates
    int num_floats = 12;
    sprite_tex_data = new GLfloat[sizeof(GLfloat)*num_floats]; 
    if(sprite_tex_data == NULL) {
        std::cerr << "ERROR in Character::generate_tex_data, cannot allocate memory" << std::endl;
        return;
    }

    GLfloat offset_x = GLfloat(TILESET_ELEMENT_SIZE) / IMAGE2_SIZE_WIDTH;
    GLfloat offset_y = GLfloat(TILESET_ELEMENT_SIZE) / IMAGE2_SIZE_HEIGHT;

    //bottom left
    sprite_tex_data[0]  = offset_x * GLfloat(4.0);
    sprite_tex_data[1]  = offset_y;

    //top left
    sprite_tex_data[2]  = offset_x * GLfloat(4.0);
    sprite_tex_data[3]  = 0.0f; 

    //bottom right
    sprite_tex_data[4]  = offset_x * GLfloat(5.0);
    sprite_tex_data[5]  = offset_y;

    //top left
    sprite_tex_data[6]  = offset_x * GLfloat(4.0);
    sprite_tex_data[7]  = 0.0f;

    //top right
    sprite_tex_data[8]  = offset_x * GLfloat(5.0);
    sprite_tex_data[9]  = 0.0f;

    //bottom right
    sprite_tex_data[10] = offset_x * GLfloat(5.0);
    sprite_tex_data[11] = offset_y;

    renderable_component.set_texture_coords_data(sprite_tex_data, sizeof(GLfloat)*num_floats, false);
} 

void Character::generate_vertex_data() {

    //holds the character vertex data
    //need 18 floats for each coordinate as these hold 3D coordinates
    int num_floats = 18;
    sprite_data  = new GLfloat[sizeof(GLfloat)*num_floats]; 
    if(sprite_data == NULL) {
        std::cerr << "ERROR in Characater::generate_vertex_data, cannot allocate memory" << std::endl;
        return;
    }

    float scale = TILESET_ELEMENT_SIZE * GLOBAL_SCALE;
    float depth = 0.0f;
    //bottom left 
    sprite_data[0] = 0;
    sprite_data[1] = 0;
    sprite_data[2] = depth;

    //top left
    sprite_data[3] = 0;
    sprite_data[4] = (1) * scale;
    sprite_data[5] = depth;

    //bottom right
    sprite_data[6] = (1) * scale;
    sprite_data[7] = 0;
    sprite_data[8] = depth;

    //top left
    sprite_data[9] = 0;
    sprite_data[10] = 1 * scale;
    sprite_data[11] = depth;

    //top right
    sprite_data[12] = 1 * scale;
    sprite_data[13] = 1 * scale;
    sprite_data[14] = depth;

    //bottom right
    sprite_data[15] = 1 * scale;
    sprite_data[16] = 0;
    sprite_data[17] = depth;

    renderable_component.set_vertex_data(sprite_data,sizeof(GLfloat)*num_floats, false);
    renderable_component.set_num_vertices_render(num_floats/3);//GL_TRIANGLES being used
}

void Character::load_textures() {

    FILE *tex_file2 = NULL;
    size_t image_sz_2 = IMAGE2_SIZE_WIDTH*IMAGE2_SIZE_HEIGHT*IMAGE2_NUM_COMPONENTS;

    tex_buf = new char[image_sz_2];

    tex_file2 = fopen("../resources/characters_1.raw", "rb");
    if(tex_file2 == NULL) {
        std::cerr << "ERROR: Couldn't load textures" << std::endl;
    }

    if (tex_file2 && tex_buf) {
        size_t bytes_read = fread(tex_buf, 1, image_sz_2, tex_file2);
        if (bytes_read != image_sz_2) {
            throw new std::runtime_error("Problems with file; wrong number of bytes read.");
        }
        fclose(tex_file2);
    }
    //Set the texture data in the rederable component
    renderable_component.set_texture_data(tex_buf, static_cast<int>(image_sz_2), IMAGE2_SIZE_WIDTH, IMAGE2_SIZE_HEIGHT, false);

}
bool Character::init_shaders() {
#ifdef USE_GLES
    //read in the shaders
    std::ifstream vertex_shader_src("vert_shader.glesv");
    std::ifstream fragment_shader_src("frag_shader.glesf");
#endif
#ifdef USE_GL
    //read in the shaders
    std::ifstream vertex_shader_src("vert_shader.glv");
    std::ifstream fragment_shader_src("frag_shader.glf");
#endif

    if (!vertex_shader_src.good()){
        std::cerr << "Failed to load vertex shader" << std::endl;
        return false;
    }
    
    if (!fragment_shader_src.good()) {
        std::cerr << "Failed to load fragment shader" << std::endl;
        return false;
    }

    std::string vert_src, frag_src, line;
    while (getline(vertex_shader_src, line)) {
        vert_src += line + std::string("\n");
    }

    while (getline(fragment_shader_src, line)) {
        frag_src += line + std::string("\n");
    }

    Shader* shader = new Shader(vert_src, frag_src);
  
    if (!shader->is_loaded()) {
        delete shader;
        shader = NULL;
        std::cerr << "Failed to create the shader" << std::endl;
        return false;
    }

    //Set the shader
    renderable_component.set_shader(shader);

    return true;

}

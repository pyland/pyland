#include "gui_manager.hpp"

#include <new>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

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

void GUIManager::parse_components() {


    //Now generate the needed rendering data
        std::cout << "Generating data::::::::::" << std::endl;

    generate_tex_data();
    generate_vertex_data();
    load_textures();

        std::cout << "DONE    ::::::::::" << std::endl;
}

void GUIManager::update_components() {
    init_shaders();
}



GUIManager::GUIManager() : gui_tex_data(nullptr), gui_data(nullptr), tex_buf(nullptr){
}

GUIManager::~GUIManager() {
    delete []gui_tex_data;
    delete []gui_data;
    delete []tex_buf;
}


void GUIManager::generate_tex_data() {
    
    //delete it if its already allocated
    delete []gui_tex_data; 

    //generate the texture data data
    std::vector<std::pair<GLfloat*, int>> components_data = root->generate_texture_data();

    //calculate data size
    long num_floats = 0;
    for(auto component_texture_data : components_data) {
        num_floats += component_texture_data.second;
    }

    //Create a buffer for the data
    try {
        gui_tex_data  = new GLfloat[sizeof(GLfloat)*num_floats]; 
    }
    catch(std::bad_alloc& ba) {
        std::cerr << "ERROR: bad_alloc caught in GUIManager::generate_tex_data()" << ba.what() << std::endl;
        return;
    }


    int gui_tex_data_offset = 0;

    //Extract the data
    for(auto component_texture_data : components_data) {
        GLfloat* texture_coords = component_texture_data.first;
        size_t texture_coords_size = size_t(component_texture_data.second);

        //copy data into buffer
        std::memcpy(&gui_tex_data[gui_tex_data_offset],texture_coords, texture_coords_size);

        gui_tex_data_offset += component_texture_data.second;
    }
    std::cout << "DONE WITH SETTING TEX COORDS" << std::endl;
//Generate the data
    renderable_component.set_texture_coords_data(gui_tex_data, sizeof(GLfloat)*num_floats, false);
} 

void GUIManager::generate_vertex_data() {
    
    //Delete the data if its already allocated
    delete []gui_data;

    //generate the vertex data
    std::vector<std::pair<GLfloat*, int>> components_data = root->generate_vertex_data();

    //calculate data size
    long num_floats = 0;
    for(auto component_vertex_data : components_data) {
        num_floats += component_vertex_data.second;
    }

    //Create a buffer for the data
    try {
        gui_data  = new GLfloat[sizeof(GLfloat)*num_floats]; 
    }
    catch(std::bad_alloc& ba) {
        std::cerr << "ERROR: bad_alloc caught in GUIManager::generate_vertex_data()" << ba.what() << std::endl;
        return;
    }


    int gui_data_offset = 0;
    //Extract the data
    for(auto component_vertex_data : components_data) {
        GLfloat* vertices = component_vertex_data.first;
        size_t vertices_size = size_t(component_vertex_data.second);

        //copy data into buffer
        std::memcpy(&gui_data[gui_data_offset], vertices, vertices_size);

        gui_data_offset += component_vertex_data.second;
    }

    
    renderable_component.set_vertex_data(gui_data,sizeof(GLfloat)*num_floats, false);
    renderable_component.set_num_vertices_render(GLsizei(num_floats/3));//GL_TRIANGLES being used
}

void GUIManager::load_textures() {


    FILE *tex_file2 = nullptr;
    size_t bytes_read = 0;
    size_t image_sz_2 = IMAGE2_SIZE_WIDTH*IMAGE2_SIZE_HEIGHT*IMAGE2_NUM_COMPONENTS;

    tex_buf = new char[image_sz_2];

    //TODO: use the actual gui texture
    tex_file2 = fopen("../resources/characters_1.raw", "rb");
    if(tex_file2 == nullptr) {
        std::cerr << "ERROR: Couldn't load textures" << std::endl;
    }

    if (tex_file2 && tex_buf) {
        bytes_read = fread(tex_buf, 1, image_sz_2, tex_file2);
        assert(bytes_read == image_sz_2);  // some problem with file?
        fclose(tex_file2);
    }
    //Set the texture data in the rederable component
    renderable_component.set_texture_data(tex_buf, static_cast<int>(image_sz_2), IMAGE2_SIZE_WIDTH, IMAGE2_SIZE_HEIGHT, false);

}
bool GUIManager::init_shaders() {
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
        vert_src += line + "\n";
    }

    while (getline(fragment_shader_src, line)) {
        frag_src += line + "\n";
    }

    Shader* shader = new Shader(vert_src, frag_src);
  
    if (!shader->is_loaded()) {
        delete shader;
        shader = nullptr;
        std::cerr << "Failed to create the shader" << std::endl;
        return false;
    }

    //Set the shader
    renderable_component.set_shader(shader);

    return true;

}





#include "component.hpp"
#include "gui_manager.hpp"
#include <new>
#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include "texture.hpp"
#include <utility>
#include <vector>

#ifdef USE_GLES

#include <GLES2/gl2.h>

#endif

#ifdef USE_GL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif

void GUIManager::parse_components() {
    //Generate  the needed offsets
    regenerate_offsets(root);

    //Now generate the needed rendering data
    generate_tex_data();
    generate_vertex_data();
    load_textures();
    init_shaders();
}

void GUIManager::regenerate_offsets(std::shared_ptr<Component> parent) {
    if(!parent)
        return;

    try{
        //Go through all the children of this component
        for(auto component_pair : parent->get_components()) {
            std::shared_ptr<Component> component = component_pair.second;

            //Recalculate the dimensions
            int width_pixels = parent->get_width_pixels();
            int height_pixels = parent->get_height_pixels();

            //calculate the pixel locations for this component, using the parent's dimensions
            component->set_width_pixels   (int(float( width_pixels) * component->get_width()));
            component->set_height_pixels  (int(float(height_pixels) * component->get_height()));
            component->set_x_offset_pixels(int(float( width_pixels) * component->get_x_offset()));
            component->set_y_offset_pixels(int(float(height_pixels) * component->get_y_offset()));

            regenerate_offsets(component);
        }
    }
    catch(component_no_children_exception& e) {
        //DONE
    }
}
void GUIManager::update_components() {

}

void GUIManager::mouse_callback_function(MouseInputEvent event) {

    //Just get the end state for the moment
    //Work out which component was clicked
    int mouse_x = event.to.x;
    int mouse_y = event.to.y;

    int curr_x_offset = 0;
    int curr_y_offset = 0;

    //Traverse the component tree
    handle_mouse_click(root, mouse_x, mouse_y, curr_x_offset, curr_y_offset);
}

bool GUIManager::handle_mouse_click(std::shared_ptr<Component> root, int mouse_x, int mouse_y, int curr_x_offset, int curr_y_offset) {
    try{
        //Go through all the children of this component
        for(auto component_pair : root->get_components()) {
            std::shared_ptr<Component> component = component_pair.second;

            //Get the component dimensions
            int component_width_pixels = component->get_width_pixels();
            int component_x_offset_pixels = component->get_x_offset_pixels();
            int component_y_offset_pixels = component->get_y_offset_pixels();
            int component_height_pixels = component->get_height_pixels();

            //The x and y offset of this component relative to the origin
            int x_offset = curr_x_offset + component_x_offset_pixels;
            int y_offset = curr_y_offset + component_y_offset_pixels;

            //The offsets which account for the width and height of the component
            int x_right_offset = x_offset + component_width_pixels;
            int y_top_offset = y_offset + component_height_pixels;

            //bounds test
            //Check to see if the click is where this component sits
            if(mouse_x >= x_offset && mouse_x <= x_right_offset &&
               mouse_y >= y_offset && mouse_y <= y_top_offset) {
                //Ok, so this component is in bounds - check if its is clickable or has children

                if(component->is_clickable()) {
                    //Call the click event handler
                    component->call_on_click();

                    //The click has been handled
                    return true;
                }

                //It's not clickable, traverse the children
                if(handle_mouse_click(component, mouse_x, mouse_y, x_offset, y_offset))  {
                    //Click has been handled
                    return true;
                }

                //Click has not been handled
                //Move onto next component in the tree (Next sibling).
            }
        }
    }
    catch(component_no_children_exception& e) {
        //Catch the exception and return false
        return false;
    }
    return false;
}

GUIManager::GUIManager() {

}

GUIManager::~GUIManager() {
}


void GUIManager::generate_tex_data() {

    //generate the texture data data
    std::vector<std::pair<GLfloat*, int>> components_data = root->generate_texture_data();

    //calculate data size
    long num_floats = 0;
    for(auto component_texture_data : components_data) {
        num_floats += component_texture_data.second;
    }

    GLfloat* gui_tex_data = nullptr;
    //Create a buffer for the data
    try {
        gui_tex_data  = new GLfloat[sizeof(GLfloat)*num_floats];
    }
    catch(std::bad_alloc& ba) {
        LOG(ERROR) << "ERROR: bad_alloc caught in GUIManager::generate_tex_data()" << ba.what();
        return;
    }


    int gui_tex_data_offset = 0;

    //Extract the data
    for(auto component_texture_data : components_data) {
        GLfloat* texture_coords = component_texture_data.first;
        size_t texture_coords_size = size_t(component_texture_data.second);

        //copy data into buffer
        std::copy(texture_coords, &texture_coords[texture_coords_size], &gui_tex_data[gui_tex_data_offset]);

        gui_tex_data_offset += component_texture_data.second;
    }

    //Generate the data
    renderable_component.set_texture_coords_data(gui_tex_data, sizeof(GLfloat)*num_floats, false);
}

void GUIManager::generate_vertex_data() {
    //generate the vertex data
    std::vector<std::pair<GLfloat*, int>> components_data = root->generate_vertex_data();

    //calculate data size
    int num_dimensions = 2;
    long num_floats = 0;
    for(auto component_vertex_data : components_data) {
        num_floats += component_vertex_data.second;
    }

    //Create a buffer for the data
    GLfloat* gui_data = nullptr;
    try {
        gui_data  = new GLfloat[sizeof(GLfloat)*num_floats];
    }
    catch(std::bad_alloc& ba) {
        LOG(ERROR) << "bad_alloc caught in GUIManager::generate_vertex_data()" << ba.what();
        return;
    }

    int gui_data_offset = 0;
    //Extract the data
    for(auto component_vertex_data : components_data) {
        GLfloat* vertices = component_vertex_data.first;
        size_t vertices_size = size_t(component_vertex_data.second);

        //copy data into buffer
        std::copy(vertices, &vertices[vertices_size], &gui_data[gui_data_offset]);

        gui_data_offset += component_vertex_data.second;
    }

    renderable_component.set_vertex_data(gui_data,sizeof(GLfloat)*num_floats, false);
    renderable_component.set_num_vertices_render(GLsizei(num_floats/num_dimensions));//GL_TRIANGLES being used
}

void GUIManager::load_textures() {
    //Set the texture data in the rederable component
    renderable_component.set_texture(TextureAtlas::get_shared("../resources/characters_1_64.png"));
}
bool GUIManager::init_shaders() {
    std::shared_ptr<Shader> shader;
    try {
        shader = Shader::get_shared("gui_shader");
    }
    catch (std::exception e) {
        LOG(ERROR) << "Failed to create the shader";
        return false;
    }

    //Set the shader
    renderable_component.set_shader(shader);

    return true;
}

#include "component.hpp"
#include "component_group.hpp"
#include "gui_text.hpp"

#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

#ifdef USE_GLES
#include <GLES2/gl2.h>
#endif

#ifdef USE_GL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif

//Include GLM
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


ComponentGroup::ComponentGroup() {

}

ComponentGroup::ComponentGroup(std::function<void (void)> on_click, float _width,
                     float _height, float _x_offset, float _y_offset) : 
    Component(on_click, _width, _height, _x_offset, _y_offset)
{

}

ComponentGroup::~ComponentGroup() {

}

std::vector<std::pair<GLfloat*,int>> ComponentGroup::generate_vertex_data() {
   std::vector<std::pair<GLfloat*, int>> group_data;

   //Call the implementation of this class  to generate it's data
   generate_this_vertex_data();

    //Add this components data
   group_data.push_back(std::make_pair(vertex_data, size_vertex_data));

    
    //Go through all the components in this group
    for(auto component : components) {
        std::vector<std::pair<GLfloat*, int>> component_vector = component.second->generate_vertex_data();
        
        //get all the pointers in the component - deals with ComponentGroup children
        for(auto component_data_pair : component_vector) {
            //comvert this into this component's local spacd

            //Calcuate how far to translate this component
            int pixel_offset_x = 0; 
            float component_x_offset = component.second->get_x_offset();
            int pixel_offset_y = 0 ;
            float component_y_offset = component.second->get_y_offset();


            pixel_offset_x =(int)((float)width_pixels * component_x_offset);
            pixel_offset_y = (int)((float)height_pixels * component_y_offset);

            //Translation matrix
            glm::mat4 transform_matrix = glm::mat4(1.0);
            glm::vec3 translate_vector = glm::vec3((float)pixel_offset_x, (float)pixel_offset_y, 0.0f);
            transform_matrix = glm::translate(transform_matrix, translate_vector);

            int size_data = component_data_pair.second;
            GLfloat* component_vertices = component_data_pair.first;
            int num_dimensions = 2;
            //translate each vertex
            for(int i = 0; i < size_data; i+=num_dimensions) {
                //Translate the vertex
                glm::vec4 vertex = glm::vec4(component_vertices[i], component_vertices[i+1], component_vertices[i+2], 1.0f);

                vertex = transform_matrix*  vertex;

                //Put the data back
                component_vertices[i] = vertex.x;
                component_vertices[i+1] = vertex.y;
            }
            
            //add to this group
            group_data.push_back(component_data_pair);
        }
    }
    return group_data;
}

std::vector<std::pair<GLfloat*, int>> ComponentGroup::generate_texture_data() {

    std::vector<std::pair<GLfloat*, int>> group_data;

   //Call the implementation of this class  to generate it's data
   generate_this_texture_data();

    //Add this components data
   group_data.push_back(std::make_pair(texture_data, size_texture_data));

   //Go through all the components in this group
    for(auto component_pair : components) {
        std::vector<std::pair<GLfloat*, int>> component_data = component_pair.second->generate_texture_data();
        
        //get all the pointers in the component - deals with ComponentGroup children
        for(auto component_vertex_ptr : component_data) {
            //add to this group
            //no translation is needed
            group_data.push_back(component_vertex_ptr);
        }
    }

    return group_data;
}

std::vector<std::shared_ptr<GUIText>> ComponentGroup::generate_text_data() {

   //Call the implementation of this class  to generate it's data
    std::vector<std::shared_ptr<GUIText>> group_data = generate_this_text_data();

   //Go through all the components in this group
    for(auto component_pair : components) {
        std::vector<std::shared_ptr<GUIText>> component_data = component_pair.second->generate_text_data();
        
        //get all the text data in the component - deals with ComponentGroup children
        for(auto text_data : component_data) {
            //comvert this into this component's local spacd
            std::cout << " MOVING" << std::endl;
            //Calcuate how far to translate this component
            int pixel_offset_x = 0; 
            float component_x_offset = text_data->get_x_offset_pixels();
            int pixel_offset_y = 0 ;
            float component_y_offset = text_data->get_y_offset_pixels();

            pixel_offset_x =(int)((float)width_pixels * component_x_offset);
            pixel_offset_y = (int)((float)height_pixels * component_y_offset);

            //Translate it
            text_data->set_x_offset_pixels(component_x_offset + pixel_offset_x);
            text_data->set_y_offset_pixels(component_y_offset + pixel_offset_y);
            std::cout << "OOFSET " << pixel_offset_x << std::endl;

            //add to this group
            group_data.push_back(text_data);
        }
    }
    return group_data;
}

void ComponentGroup::add(std::shared_ptr<Component> component) {
    components[component->get_id()] = component;
    component->set_parent(this);

    //We calculate the pixel display dimensions 

    //calculate the pixel locations for this component, using the parent's dimensions
    component->set_width_pixels((int)((float)width_pixels*component->get_width()));
    component->set_height_pixels((int)((float)height_pixels*component->get_height()));
    component->set_x_offset_pixels((int)((float)width_pixels*component->get_x_offset()));
    component->set_y_offset_pixels((int)((float)height_pixels*component->get_y_offset()));                }

void ComponentGroup::remove(int component_id) {
    std::shared_ptr<Component> component = components[component_id];
    component->set_parent(nullptr);
    components.erase(component_id);
        //no longer the parent
}

std::shared_ptr<Component> ComponentGroup::get_component(int component_id) {
    std::shared_ptr<Component> component = nullptr;

    //Get the component if its in the map and handle it if it doesn't exist
    try {
        component = components.at(component_id);
    } catch(const std::out_of_range& oor) {
        std::cerr << "COMPONENT GROUP no such component " << component_id << std::endl;
    }
    return component;
}

const std::map<int, std::shared_ptr<Component>>& ComponentGroup::get_components() {
    return components;
}

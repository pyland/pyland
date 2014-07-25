
#include "component.hpp"
#include "component_group.hpp"

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

ComponentGroup::~ComponentGroup() {

}

std::vector<std::pair<GLfloat*,int>> ComponentGroup::generate_vertex_data() {
   std::vector<std::pair<GLfloat*, int>> group_data;
    
    //Go through all the components in this group
    for(auto component_pair : components) {
        std::vector<std::pair<GLfloat*, int>> component_data = component_pair.second->generate_texture_data();
        
        //get all the pointers in the component - deals with ComponentGroup children
        for(auto component_vertex_ptr : component_data) {
            //comvert this into this component's local spacd

            //Calcuate how far to translate this component
            int pixel_offset_x = 0; 
            float component_x_offset = component_pair.second->get_x_offset();
            int pixel_offset_y = 0 ;
            float component_y_offset = component_pair.second->get_y_offset();

            pixel_offset_x = (float)width_pixels * component_x_offset;
            pixel_offset_y = (float)height_pixels * component_y_offset;

            //Translate it
            glm::mat4 transform = glm::mat4(1.0);
            glm::vec4 translate = glm::vec4((float)pixel_offset_x, (float)pixel_offset_y, 0.0f, 0.0f);

            //translate each vertex
            
            
            //add to this group
            group_data.push_back(component_vertex_ptr);
        }
    }

    return group_data;
}
std::vector<std::pair<GLfloat*, int>> ComponentGroup::generate_texture_data() {

    std::vector<std::pair<GLfloat*, int>> group_data;
    
    //Go through all the components in this group
    for(auto component_pair : components) {
        std::vector<std::pair<GLfloat*, int>> component_data = component_pair.second->generate_texture_data();
        
        //get all the pointers in the component - deals with ComponentGroup children
        for(auto component_vertex_ptr : component_data) {
            //add to this group
            group_data.push_back(component_vertex_ptr);
        }
    }

    return group_data;
}
void ComponentGroup::add(std::shared_ptr<Component> component) {
    components[component->get_id()] = component;
}


void ComponentGroup::remove(int component_id) {
    components.erase(component_id);
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

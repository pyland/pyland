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


ComponentGroup::ComponentGroup() {

}

ComponentGroup::~ComponentGroup() {

}

std::vector<std::pair<std::shared_ptr<GLfloat>,int>> ComponentGroup::generate_vertex_data() {
   std::vector<std::pair<std::shared_ptr<GLfloat>, int>> group_data;
    
    //Go through all the components in this group
    for(auto component : components) {
        std::vector<std::pair<std::shared_ptr<GLfloat>, int>> component_data = component.generate_texture_data();
        
        //get all the pointers in the component - deals with ComponentGroup children
        for(auto component_vertex_ptr : component_data) {
            //add to this group
            group_data.push_back(component_vertex_ptr);
        }
    }

    return group_data;
}
std::vector<std::pair<std::shared_ptr<GLfloat>, int>> ComponentGroup::generate_texture_data() {

    std::vector<std::pair<std::shared_ptr<GLfloat>, int>> group_data;
    
    //Go through all the components in this group
    for(auto component : components) {
        std::vector<std::pair<std::shared_ptr<GLfloat>, int>> component_data = component.generate_texture_data();
        
        //get all the pointers in the component - deals with ComponentGroup children
        for(auto component_vertex_ptr : component_data) {
            //add to this group
            group_data.push_back(component_vertex_ptr);
        }
    }

    return group_data;
}
void ComponentGroup::add_component(std::shared_ptr<Component> component) {
    components[component->get_id()] = component;
}


void ComponentGroup::remove_component(int component_id) {
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

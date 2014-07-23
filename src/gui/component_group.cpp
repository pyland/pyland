#include "component.hpp"
#include "component_group.hpp"

#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>

ComponentGroup::ComponentGroup() {

}

ComponentGroup::~ComponentGroup() {

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

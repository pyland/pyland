#include "component.hpp"

void Component::generate_vertex_data(Component& component) {

}

void Component::generate_texture_data(Component& component) {
    
}

bool Component::set_on_click(std::function<void (void)> func) {
    on_click_func= func;
}

void Component::clear_on_click() {
    auto function = [] () {return; };
    on_click_func = function;
}

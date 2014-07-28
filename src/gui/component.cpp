#include "component.hpp"

#include <vector>
Component::Component() : vertex_data(nullptr), texture_data(nullptr),
                  size_vertex_data(0), size_texture_data(0),
                  id(0), width(0.0f), height(0.0f),
                  width_pixels(0),height_pixels(0),
                  x_offset(0.0f), y_offset(0.0f) {

}

Component::~Component() {
    delete []vertex_data;
    delete []texture_data;
}

bool Component::set_on_click(std::function<void (void)> func) {
    on_click_func= func;
}

void Component::clear_on_click() {
    auto function = [] () {return; };
    on_click_func = function;
}

#include "component.hpp"

#include <vector>

Component::Component() {

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

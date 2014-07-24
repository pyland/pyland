#include "component.hpp"

#include <vector>

bool Component::set_on_click(std::function<void (void)> func) {
    on_click_func= func;
}

void Component::clear_on_click() {
    auto function = [] () {return; };
    on_click_func = function;
}

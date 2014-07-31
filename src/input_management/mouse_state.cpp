#include <ostream>

#include "mouse_state.hpp"



MouseState::MouseState(int x, int y, int buttons):
    x(x),
    y(y),
    buttons(buttons) {
}

bool MouseState::is_down(int button) {
    return ((buttons >> (button - 1)) & 1) != 0;
}

std::ostream& operator<<(std::ostream& stream, const MouseState& state) {
    return stream << "MouseState(x="       << state.x
                           << ", y="       << state.y
                           << ", buttons=" << state.buttons << ")";
}

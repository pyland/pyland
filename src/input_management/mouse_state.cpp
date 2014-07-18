#include "mouse_state.hpp"



MouseState::MouseState(int x, int y, MouseState::ButtonMask buttons):
    x(x),
    y(y),
    buttons(buttons) {
}

#include "graphics_context.hpp"
#include "game_window.hpp"



GraphicsContext* GraphicsContext::current = nullptr;



GraphicsContext::GraphicsContext(GameWindow* window):
    window(window) {
}


bool GraphicsContext::operator==(GraphicsContext other) {
    return window == other.window;
}


void GraphicsContext::use() {
    // window will set the current context.
    window->use_context();
}


GraphicsContext* GraphicsContext::get_current() {
    return current;
}

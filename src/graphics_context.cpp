extern "C" {
#ifdef USE_GL
#include <SDL2/SDL.h>
#endif

#ifdef USE_GLES
#include <EGL/egl.h>
#include <EGL/eglext.h>
#endif
}



#include "game_window.hpp"



GraphicsContext::GraphicsContext(GameWindow* window):
    window(window) {
}


bool GraphicsContext::operator==(GraphicsContext other) {
    return window == other.window;
}


void GraphicsContext::use() {
    window->use_context();
}

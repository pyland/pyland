#include <iostream>

extern "C" {
#include <SDL.h>
  
#ifdef USE_GLES
#include <GLES2/gl2.h>
#endif

#ifdef USE_GL
#include <GL/gl.h>
#endif
}

#include "game_window.hpp"
#include "input_manager.hpp"


int main(int argc, char** argv) {
    std::cerr << "Startup..." << std::endl;
    
    try {
        GameWindow window = GameWindow(640, 400, false);
        InputManager* input_manager = window.get_input_manager();
        window.use_context();
        glClearColor(0.25f, 0.50f, 1.0f, 1.0f);
        float r, g, b;
        r = g = b = 0.0f;
        while (window.check_close() == false) {
            // Little basic colour change test.
            r += 0.001;
            g += 0.01;
            b += 0.1;
            if (r >= 1.0f) r = 0.0f;
            if (g >= 1.0f) g = 0.0f;
            if (b >= 1.0f) b = 0.0f;
            glClearColor(r, g, b, 0.8f);
            glClear(GL_COLOR_BUFFER_BIT);
            window.swap_buffers();
            SDL_Delay(0);
            GameWindow::update();
            if (input_manager->is_key_down(SDLK_ESCAPE) && input_manager->is_key_down(SDLK_LSHIFT)) {
                std::cerr << "Shift+Escape pressed!" << std::endl;
                break;
            }
        }
    }
    catch (GameWindow::InitException e) {
        std::cerr << e.what() << std::endl;
    }

    std::cerr << "Terminate" << std::endl;
    
    return 0;
}

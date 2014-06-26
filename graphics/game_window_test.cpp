#include <iostream>

extern "C" {
#include <SDL.h>
#include <bcm_host.h>
}

#include "game_window.hpp"


int main(int argc, char** argv) {
    std::cerr << "Startup...";
    
    bcm_host_init ();
    
    try {
        GameWindow window = GameWindow(640, 400, false);
        window.use_context();
        glClearColor(0.25f, 0.50f, 1.0f, 1.0f);
        while (window.check_close() == false) {
            glClear(GL_COLOR_BUFFER_BIT);
            window.swap_buffers();
            SDL_Delay(100);
            window.update();
        }
    }
    catch (GameWindow::InitException e) {
        std::cerr << e.what() << std::endl;
    }

    std::cerr << "Terminate";
    
    return 0;
}

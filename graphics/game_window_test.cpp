#include <iostream>

extern "C" {
#include <SDL.h>
#include <bcm_host.h>
}

#include "game_window.hpp"


int main(int argc, char** argv) {
    bcm_host_init ();
    
    try {
        GameWindow window = GameWindow(640, 400, false);
        SDL_Delay(3000);
    }
    catch (GameWindow::InitException e) {
        std::cerr << e.what() << std::endl;
    }
    
    return 0;
}

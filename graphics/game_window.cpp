#include <iostream>

extern "C" {
#include <SDL.h>
}

#include "game_window.hpp"



/**
 *  Count of how many instances exist.
 *
 *  This is used to manage SDL (de)initialisation.
 */
static int window_count = 0;



GameWindow::InitException::InitException (const char* message) {
    this->message = message;
}

const char* GameWindow::InitException::what () {
    return message;
}



GameWindow::GameWindow (int width, int height, bool fullscreen) {
    if (window_count == 0) {
        init_sdl(); // May throw InitException
    }

    // SDL already uses width,height = 0,0 for automatic
    // resolution. Sets maximized if not in fullscreen and given
    // width,height = 0,0.
    window = SDL_CreateWindow ("Project Zygote",
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               width,
                               height,
                               SDL_WINDOW_OPENGL
                               | (fullscreen ? SDL_WINDOW_FULLSCREEN : SDL_WINDOW_RESIZABLE)
                               | ( (!fullscreen && width == 0 && height == 0) ?
                                   SDL_WINDOW_MAXIMIZED : 0 )
                               );

    if (window == NULL) {
#ifdef DEBUG
        std::cerr << "Failed to create SDL window." << std::endl;
#endif
        deinit_sdl();
        throw GameWindow::InitException("Failed to create SDL window");
    }
    
    window_count++;
}

GameWindow::~GameWindow () {
    SDL_DestroyWindow (window);
    
    window_count--;
    if (window_count == 0) {
        deinit_sdl();
    }
}

void GameWindow::update () {
}



void GameWindow::init_sdl () {
    int result;
    
#ifdef DEBUG
    std::cerr << "Initializing SDL..." << std::endl;
#endif
    
    result = SDL_Init (SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  
    if (result != 0) {
        throw GameWindow::InitException("Failed to initialize SDL");
    }
    
#ifdef DEBUG
    std::cerr << "SDL initialized." << std::endl;
#endif
}

void GameWindow::deinit_sdl () {
#ifdef DEBUG
    std::cerr << "Deinitializing SDL..." << std::endl;
#endif
    
    // Should always work.
    SDL_Quit ();
    
#ifdef DEBUG
    std::cerr << "SDL deinitialized." << std::endl;
#endif
}

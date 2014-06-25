#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H

#include <exception>

extern "C" {
#include <SDL.h>
}

/**
 *  Sets up OpenGL|ES and handles SDL functionality.
 */
class GameWindow {
private:
    /**
     *  Stores the SDL window.
     */
    SDL_Window* window;
    
    /**
     *  SDL window and EGL surface width.
     */
    int window_width;
    /**
     *  SDL window and EGL surface height.
     */
    int window_height;
    
    /**
     *  Initialize SDL.
     */
    static void init_sdl();

    /**
     *  Deinitialize SDL.
     */
    static void deinit_sdl();

public:
    class InitException: public std::exception {
    protected:
        const char* message;
    public:
        InitException (const char* message);
        virtual const char* what ();
    };

    /**
     *  Creates an SDL window and initializes OpenGL|ES.
     *
     *  @param width The width of the window. 0 uses current resolution.
     *  @param height The height of the window. 0 uses current resolution.
     *  @param fullscreen Whether to use fullscreen.
     */
    GameWindow(int width, int height, bool fullscreen = false);

    /**
     *  Shuts down and cleans up both SDL and EGL.
     */
    ~GameWindow();

    /**
     *  Process events, including handling of window events.
     */
    void update();
};

#endif

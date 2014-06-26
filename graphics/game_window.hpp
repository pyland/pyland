#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H

#include <exception>

extern "C" {
#include <GLES/gl.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <SDL.h>
}

/**
 *  Sets up OpenGL|ES and handles SDL functionality.
 */
class GameWindow {
private:
    /**
     *  Used to say if something need to be initialized, deinitialized,
     *  or if no action needs to be taken.
     */
    enum class InitAction {
        DO_NOTHING,
        DO_INIT,
        DO_DEINIT
    };
    
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

    bool visible;

    bool close_requested;

    /**
     * Tracks whether the egl surface needs to be changed
     */
    InitAction change_surface;
    
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;

    EGLConfig config;
    EGLint configCount;
    
    /**
     *  Initialize SDL.
     */
    void init_sdl();
    /**
     *  Deinitialize SDL.
     */
    void deinit_sdl();
    
    /**
     *  Initialize EGL. Done once per window.
     */
    void init_gl();
    /**
     *  Deinitialize EGL. Done once per window.
     */
    void deinit_gl();

    /**
     *  Creates the EGL surface.
     *
     *  This can be done whenever the surface needs to change position
     *  or resize.
     */
    void init_surface();
    /**
     *  Destroys the EGL surface.
     *
     *  This is used as a clean up function, but is also used when the
     *  window needs to resize/relocate (or even minimize).
     */
    void deinit_surface();

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
     *
     *  SDL events are all done in one go, so they are all done here.
     */
    static void update();

    /**
     *  Sets a flag signalling a close request to true.
     *
     *  This is meant as a way to signal that the user has requested the
     *  window to close, so that other parts of the program (which use
     *  the window) can perform their own stuff (like confirmation).
     */
    void request_close();

    /**
     *  Cancels any close requests, setting the flag to false.
     */
    void cancel_close();

    /**
     *  Checks whether the window has a close request.
     */
    bool check_close();
};

#endif

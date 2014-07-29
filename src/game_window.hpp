#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H

#include <stdexcept>
#include <map>
#include <utility>

// Flags to allow code to run on standard OpenGL systems.
#ifdef USE_GL
#undef USE_GLES
#else
#ifndef USE_GLES
#define USE_GLES
#endif
#endif

extern "C" {
#ifdef USE_GLES
#include <EGL/egl.h>
#include <EGL/eglext.h>
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
}

#include "callback.hpp"
#include "callback_registry.hpp"
#include "lifeline.hpp"
#include "lifeline_controller.hpp"

class InputManager;

///
/// Sets up OpenGL|ES and handles basic SDL functionality.
///
/// Input management is handled in a separate class.
///
class GameWindow {
private:
    friend class InputManager;
    ///
    /// Used to say if something needs to be initialized, deinitialized,
    /// or if no action needs to be taken.
    ///
    enum class InitAction {
        DO_NOTHING,
        DO_INIT,
        DO_DEINIT
    };
    
    ///
    /// Stores the SDL window.
    ///
    SDL_Window* window;
    
    ///
    /// Stores the SDL renderer.
    ///
    /// The renderer can be used to draw things without the need to go
    /// through opengl textures and rendering (e.g. fonts).
    ///
    SDL_Renderer* renderer;
    
    ///
    /// SDL window and EGL surface width.
    ///
    int window_width;
    ///
    /// SDL window and EGL surface height.
    ///
    int window_height;

    ///
    /// SDL window and EGL surface displacement from XServer left edge.
    ///
    int window_x;
    ///
    /// SDL window and EGL surface displacement from XServer top edge.
    ///
    int window_y;

    ///
    /// Keeps track of whether the surface is visible (i.e. exists)
    ///
    bool visible;

    ///
    /// If the window is being resized (true only during an update).
    ///
    bool resizing;

    ///
    /// Whether ths window is waiting to close
    ///
    /// If the window has been requested to close (e.g. through the
    /// window manager), this will be set to true. This can be
    /// cancelled, such as by a confirmation dialog. The close will
    /// occur when the game window is deleted.
    ///
    bool close_requested;

    ///
    ///Tracks whether the egl surface needs to be changed
    ///
    InitAction change_surface;

#ifdef USE_GLES
    // These need to be reused for resource management.
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;

    EGLConfig config;
    EGLint configCount;

    DISPMANX_DISPLAY_HANDLE_T dispmanDisplay;
    DISPMANX_ELEMENT_HANDLE_T dispmanElement;
    
    ///
    /// Overscan compensation (left border pixels)
    ///
    static int overscan_left;
    ///
    /// Overscan compensation (top border pixels)
    ///
    static int overscan_top;
#endif
#ifdef USE_GL
    SDL_GLContext sdl_gl_context;
#endif

    ///
    /// Mapping of SDL window IDs to GameWindows.
    ///
    static std::map<Uint32,GameWindow*> windows;

    ///
    /// The currently focused window
    ///
    static GameWindow* focused_window;
    ///
    /// Stores X11 display and window information.
    ///
    SDL_SysWMinfo wm_info;

    ///
    /// Handle all the input separately to all the display setup.
    ///
    InputManager* input_manager;

    ///
    /// Callback registry for window resizes.
    ///
    CallbackRegistry<void,GameWindow*> resize_callbacks;
    ///
    /// Upon destruction, callback lifelines should be disabled.
    ///
    LifelineController callback_controller;
    
    ///
    /// Initialize SDL.
    ///
    void init_sdl();
    ///
    /// Deinitialize SDL.
    ///
    void deinit_sdl();
    
    ///
    /// Initialize EGL. Done once per window.
    ///
    void init_gl();
    ///
    /// Deinitialize EGL. Done once per window.
    ///
    void deinit_gl();

    ///
    /// Creates the EGL surface.
    ///
    /// This can be done whenever the surface needs to change position
    /// or resize.
    ///
    /// @param x x position of surface
    /// @param y y position of surface
    /// @param w width of surface
    /// @param h height of surface
    ///
    void init_surface(int x, int y, int w, int h);
    ///
    /// Creates the EGL surface. (Gets width and height from X window)
    ///
    void init_surface();
    ///
    /// Destroys the EGL surface.
    ///
    /// This is used as a clean up function, but is also used when the
    /// window needs to resize/relocate (or even minimize or hide).
    ///
    void deinit_surface();

public:
    ///
    /// Used when SDL or EGL code fails to initialize, reinitialize, or
    /// deinitialize.
    ///
    class InitException: public std::runtime_error {
    public:
        InitException(const char  *message);
        InitException(const std::string &message);
    };

    ///
    /// Creates an SDL window and initializes OpenGL|ES.
    ///
    /// @param width The width of the window. 0 uses current resolution.
    /// @param height The height of the window. 0 uses current resolution.
    /// @param fullscreen Whether to use fullscreen.
    ///
    GameWindow(int width, int height, bool fullscreen = false);

    ///
    /// Shuts down and cleans up both SDL and EGL.
    ///
    ~GameWindow();

    ///
    /// Process events and changes, including handling of window events.
    ///
    /// SDL events are all done in one go, so they are all done here.
    ///
    static void update();

    ///
    /// Sets a flag signalling a close request to true.
    ///
    /// This is meant as a way to signal that the user has requested the
    /// window to close, so that other parts of the program (which use
    /// the window) can perform their own stuff (like confirmation).
    ///
    void request_close();

    ///
    /// Cancels any close requests, setting the flag to false.
    ///
    void cancel_close();

    ///
    /// Checks whether the window has a close request.
    ///
    bool check_close();

    ///
    /// Get the width and height of the window.
    ///
    /// @warning
    ///     This method is subject to API changes.
    ///
    /// Returns the dimensions of the renderable area (excluding window
    /// borders).
    ///
    std::pair<int, int> get_size();

    ///
    /// Switches the active opengl context to the one for this window.
    ///
    /// This should always be run before code using OpenGL calls runs.
    ///
    void use_context();

    ///
    /// Swaps the opengl buffers for this window.
    ///
    void swap_buffers();

    ///
    /// Input manager getter.
    ///
    InputManager* get_input_manager();

    ///
    /// Convert a pair of pixels coordinates into a pair of ratio
    /// coordinates of the distance from the bottom-left to the
    /// top-right.
    ///
    /// @param  pixels The coordinates as a pair (x first then y) of
    ///         integers from the bottom-left.
    /// @return a pair of floats (x first then y) from 0 to 1
    ///         representing the fraction of the distance across the
    ///         window from the bottom-left to the top-right.
    ///
    std::pair<float,float> get_ratio_from_pixels(std::pair<int,int> pixels);

    ///
    /// Registers a callback function for window resize handling.
    ///
    /// @param callback Callback to be used to handle the resize.
    ///
    void register_resize_handler(Callback<void, GameWindow*> callback);
    ///
    /// Registers a callback function for window resize handling.
    ///
    /// @param func Callback function to be used to handle the resize.
    /// @return A lifeline which keeps the callback active.
    ///
    Lifeline register_resize_handler(std::function<void(GameWindow*)> func);
};



#endif

/*
 * Author: Ashley Newson
 */



/**
 * Sets up OpenGL|ES and handles SDL functionality.
 */

class WindowedGL {
private:
    /**
     *   SDL window and EGL surface width.
     */
    int window_width;
    /**
     *   SDL window and EGL surface height.
     */
    int window_height;
    
    /**
     *   Displacement of SDL window from XServer left edge.
     */
    int window_x;
    /**
     *   Displacement of SDL window from XServer top edge.
     */
    int window_y;

    // All of these are needed to allow dynamic window resizing.
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    EGLConfig  config;

    /**
     *   Stores information about the SDL window's location.
     */
    SDL_SysWMinfo wm_info;

    /**
     *   Initialize SDL.
     */
    void setup_sdl();

    /**
     *   Recreates the EGL surface to match the SDL window.
     *
     *   Note that this is not perfect, as the origin of the XServer
     *   might not be aligned with the origin of the framebuffer, which
     *   causes a displacement (typically small) from the window's area.
     */
    void reset_surface();

public:
    /**
     *   Starts SDL in fullscreen and initializes OpenGL|ES.
     */
    WindowedGL();
    
    /**
     *   Creates an SDL window and initializes OpenGL|ES.
     *
     *   @param width The width of the window.
     *   @param height The height of the window.
     */
    WindowedGL(int width, int height);

    /**
     *   Shuts down and cleans up both SDL and EGL.
     */
    ~WindowedGL();

    /**
     *   Process events, including handling of window events.
     */
    void update();
}

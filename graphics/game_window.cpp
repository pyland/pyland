#include <iostream>
#include <map>

extern "C" {
#include <GLES/gl.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <SDL.h>
}

#include "game_window.hpp"



// DEFUNCT
/**
 *  Count of how many instances exist.
 *
 *  This is used to manage SDL (de)initialisation.
 */
static int window_count = 0;

/**
 *  Mapping of SDL window IDs to GameWindows.
 */
static std::map<Uint32,GameWindow*> windows = std::map<Uint32,GameWindow*>();



GameWindow::InitException::InitException(const char* message) {
    this->message = message;
}


const char* GameWindow::InitException::what() {
    return message;
}



GameWindow::GameWindow(int width, int height, bool fullscreen) {
    visible = false;
    
    if (windows.size() == 0) {
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
    
    // window_count++;
    //windows.insert( std::pair<Uint32,GameWindow*>(SDL_GetWindowID(window), this) );
    windows[SDL_GetWindowID(window)] = this;
}

GameWindow::~GameWindow() {
    SDL_DestroyWindow (window);
    
    // window_count--;
    windows.erase(SDL_GetWindowID(window));
    if (windows.size() == 0) {
        deinit_sdl();
    }
}


void GameWindow::init_sdl() {
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


void GameWindow::deinit_sdl() {
#ifdef DEBUG
    std::cerr << "Deinitializing SDL..." << std::endl;
#endif
    
    // Should always work.
    SDL_Quit ();
    
#ifdef DEBUG
    std::cerr << "SDL deinitialized." << std::endl;
#endif
}


void GameWindow::init_gl() {
    EGLBoolean result;

    static const EGLint attributeList[] = {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT, /* (???) */
        EGL_NONE
    };

    // Get an EGL display connection

    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
#ifdef DEBUG
        std::cerr << "Error getting display." << std::endl;
#endif
        throw GameWindow::InitException("Error getting display");
    }

    // Initialize EGL display connection
    result = eglInitialize(display, NULL, NULL);
    if (result == EGL_FALSE) {
#ifdef DEBUG
        std::cerr << "Error initializing display connection." << std::endl;
#endif
        throw GameWindow::InitException("Error initializing display connection");
    }

    // Get frame buffer configuration
    result = eglChooseConfig(display, attributeList, &config, 1, &configCount);
    if (result == EGL_FALSE) {
#ifdef DEBUG
        std::cerr << "Error getting frame buffer configuration." << std::endl;
#endif
        throw GameWindow::InitException("Error getting frame buffer configuration");
    }

    // Create EGL rendering context
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, NULL);
    if (context == EGL_NO_CONTEXT) {
#ifdef DEBUG
        std::cerr << "Error creating rendering context." << std::endl;
#endif
        throw GameWindow::InitException("Error creating rendering context");
    }

    // Surface initialization is done here as it can be called multiple
    // times after  main initialization.
    init_surface();
}


void GameWindow::deinit_gl() {
    // Release OpenGL resources
    deinit_surface();
    eglDestroyContext(display, context);
    eglTerminate(display);
}


void GameWindow::init_surface() {
    EGLBoolean result;
  
    VC_RECT_T destination;
    VC_RECT_T source;
  
    static EGL_DISPMANX_WINDOW_T nativeWindow;
    DISPMANX_ELEMENT_HANDLE_T dispmanElement;
    DISPMANX_DISPLAY_HANDLE_T dispmanDisplay;
    DISPMANX_UPDATE_HANDLE_T dispmanUpdate;

#ifdef DEBUG
    std::cerr << "Recreating window surface." << std::endl;
#endif
  
    // Create EGL window surface

    int x, y, w, h;

    SDL_GetWindowPosition(window, &x, &y);
    destination.x = x;
    destination.y = y;
    SDL_GetWindowSize(window, &w, &h);
    destination.width = w;
    destination.height = h;
    source.x = 0;
    source.y = 0;
    source.width  = w << 16; // (???)
    source.height = h << 16; // (???)

    dispmanDisplay = vc_dispmanx_display_open(0); // (???)
    dispmanUpdate  = vc_dispmanx_update_start(0); // (???)

    dispmanElement = vc_dispmanx_element_add(dispmanUpdate, dispmanDisplay,
                                             0/*layer*/, &destination, 0/*src*/,
                                             &source, DISPMANX_PROTECTION_NONE,
                                             0 /*alpha*/, 0/*clamp*/, (DISPMANX_TRANSFORM_T)0/*transform*/); // (???)
  
    nativeWindow.element = dispmanElement;
    nativeWindow.width = w; // (???)
    nativeWindow.height = h; // (???)
    vc_dispmanx_update_submit_sync(dispmanUpdate); // (???)
    
    if (visible) {
        eglDestroySurface(display, surface);
    }
    
    surface = eglCreateWindowSurface(display, config, &nativeWindow, NULL);
    if (surface == EGL_NO_SURFACE) {
#ifdef DEBUG
        std::cerr << "Error creating window surface." << std::endl;
#endif
        throw GameWindow::InitException("Error creating window surface");
    }

    // Connect the context to the surface
    result = eglMakeCurrent(display, surface, surface, context);
    if (result == EGL_FALSE) {
#ifdef DEBUG
        std::cerr << "Error connecting context to surface." << std::endl;
#endif
        throw GameWindow::InitException("Error connecting context to surface");
    }

    visible = true;
}


void GameWindow::deinit_surface() {
    if (visible) {
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroySurface(display, surface);
    }
    visible = false;
}


void GameWindow::update() {
    // Instead of reinitialising on every event, do it ater we have
    // scanned the event queue in full.
    SDL_Event event;
    
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_WINDOWEVENT:
            GameWindow* window = windows[event.window.windowID];

            // Should focus events be included?
            switch (event.window.event) {
            case SDL_WINDOWEVENT_CLOSE:
                window->request_close();
                break;
            case SDL_WINDOWEVENT_MOVED:
            case SDL_WINDOWEVENT_RESIZED:
            case SDL_WINDOWEVENT_RESTORED:
            case SDL_WINDOWEVENT_MAXIMIZED:
            case SDL_WINDOWEVENT_SHOWN:
                window->change_surface = GameWindow::InitAction::DO_INIT;
                break;
            case SDL_WINDOWEVENT_MINIMIZED:
            case SDL_WINDOWEVENT_HIDDEN:
                window->change_surface = GameWindow::InitAction::DO_DEINIT;
                break;
            }
            break;
        }
    }

    for (auto pair : windows) {
        GameWindow* window = pair.second;
        switch (window->change_surface) {
        case GameWindow::InitAction::DO_INIT:
            window->init_surface();
            break;
        case GameWindow::InitAction::DO_DEINIT:
            window->deinit_surface();
            break;
        }
    }
}


void GameWindow::request_close() {
    close_requested = true;
}


void GameWindow::cancel_close() {
    close_requested = false;
}


bool GameWindow::check_close() {
    return close_requested;
}

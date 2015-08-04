// //////////////////////////////////////////////////////////////
// //////////////////////// CURRENT BUGS ////////////////////////
// //////////////////////////////////////////////////////////////
//  First window focus events are lost:
//      Temporary fix in constructor function.
//      This is only a problem on GLES platforms.
//

// Behaviour modifiers (defines):
//  STATIC_OVERSCAN:
//      Set overscan compensation by force, don't query it.
//      Sets OVERSCAN_LEFT=24 and OVERSCAN_TOP=16 by default.
//  OVERSCAN_LEFT, OVERSCAN_TOP:
//      Set default (or fixed) overscan.
//  GAME_WINDOW_DISABLE_DIRECT_RENDER:
//      Never render directly to the screen - always use a PBuffer.
//      This is primarily for debugging purposes. It will decrease
//      performance signifficantly.
//

#include "interpreter.hpp"

#include <QDataStream>
#include <QMetaType>
#include <QTextStream>
#include <QCursor>
#include <QStyleOption>
#include <qcoreevent.h>

#include <fstream>
#include <glog/logging.h>
#include <map>
#include <string>
#include <utility>

// Include position important.
#include "game_window.hpp"
#include "input_manager.hpp"

extern "C" {
#include <SDL2/SDL.h>

#ifdef USE_GLES
#include <SDL2/SDL_syswm.h>
#include <bcm_host.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <X11/Xlib.h>
#endif
}

#include "callback.hpp"
#include "callback_registry.hpp"
#include "lifeline.hpp"
#include "lifeline_controller.hpp"
#include "graphics_context.hpp"

#ifdef USE_GLES

#ifdef STATIC_OVERSCAN

#ifndef OVERSCAN_LEFT
#define OVERSCAN_LEFT 24
#endif
#ifndef OVERSCAN_TOP
#define OVERSCAN_TOP  16
#endif

#else

#ifndef OVERSCAN_LEFT
#define OVERSCAN_LEFT 24
#endif
#ifndef OVERSCAN_TOP
#define OVERSCAN_TOP  16
#endif

#endif
int GameWindow::overscan_left = OVERSCAN_LEFT;
int GameWindow::overscan_top  = OVERSCAN_TOP;

#endif

//New include calls
//#include <QApplication>
#include <QApplication>
#include "game_init.hpp"
#include "game_main.hpp"
#include "mainwindow.h"
#include "parsingfunctions.hpp"

std::map<Uint32,GameWindow*> GameWindow::windows = std::map<Uint32,GameWindow*>();
GameWindow* GameWindow::focused_window = nullptr;

// Need to inherit constructors manually.
// NOTE: This will, and are required to, copy the message.
GameWindow::InitException::InitException(const char *message): std::runtime_error(message) {}
GameWindow::InitException::InitException(const std::string &message): std::runtime_error(message) {}

#ifdef USE_GLES
#include <boost/regex.hpp>

///
/// Queries the overscan values to compensate the window position.
///
/// For a VERY short time, this TRIED to use vcgencmd, but it turns out
/// that is very good at ignoring disable_overscan.
/// For now, we are forced to read the /boot/config.txt file.
///
static std::pair<int, int> query_overscan(int left, int top) {
    static const boost::regex match_line(
        "(disable_overscan|overscan_left|overscan_top)=(\\d+)"
        "(?:\\s.*)?" // Optionally allow anything else after a space
    );
    boost::smatch match;

    std::map<std::string, int> values({
        { "disable_overscan", 0    },
        { "overscan_left",    left },
        { "overscan_top",     top  }
    });

    std::ifstream boot_config_file("/boot/config.txt");

    if (boot_config_file.fail()) {
        LOG(ERROR) << "Unable to query overscan using /boot/config.txt. Using defaults.";
        return std::make_pair(values["overscan_left"], values["overscan_top"]);
    }

    std::string line;
    while (std::getline(boot_config_file, line)) {
        if (boost::regex_match(line, match, match_line)) {
            try {
                values[match[1]] = std::stoi(match[2]);
            }
            catch (std::exception &) {
                // Just ignore
                LOG(ERROR) << "Unparseable line: " << line;
            }
        }
    }

    if (values["disable"]) {
        LOG(INFO) << "Overscan is disabled - compensation set to (0, 0).";
        return std::make_pair(0, 0);
    }
    else {
        LOG(INFO) << "Overscan is enabled - compensation set to "
                  << values["overscan_left"] << ", " << values["overscan_top"];

        return std::make_pair(values["overscan_left"], values["overscan_top"]);
    }
}
#endif

GameWindow::GameWindow(int width, int height, int &argc, char **argv, GameMain *exGame):
    window_width(width),
    window_height(height),
    window_x(0),
    window_y(0),
    visible(false),
#ifdef GAME_WINDOW_DISABLE_DIRECT_RENDER
    foreground(false),
#else
    foreground(true),
    was_foreground(foreground),
#endif
    resizing(false),
    close_requested(false),
    graphics_context(this)
{
    LOG(INFO) << "Creating GameWindow... " << std::endl;
    input_manager = new InputManager(this);

    if (windows.size() == 0) {
        init_sdl(); // May throw InitException
    }

    LOG(INFO) << "Creating QApplication..." << std::endl;

    bool new_api = false; //TODO: Change this so that it is a command line argument
    if(new_api){
        create_apih_from_wrapper();
    }
    app = new QApplication(argc,argv);
    app->setStyle("gtk");
    app->setAttribute(Qt::AA_NativeWindows, true);

    mainWin = new MainWindow(exGame);

     //Get the SDL window from the widget in the QT interface, so it can be drawn to in game_main
    window = mainWin->getSDLWindow();
    LOG(INFO) << "Created QApplication" << std::endl;

//#ifdef USE_GL
//                   | SDL_WINDOW_OPENGL
//#endif
//                               );
    if (window == nullptr) {
        LOG(ERROR) << "Failed to create SDL window.";
        deinit_sdl();
        throw GameWindow::InitException("Failed to create SDL window");
    }

    SDL_ShowCursor(1);

    // Temporary fix (which just seems to work) for a bug where focus
    // events are not generated for the first time focus is changed.
    // SEE ALSO BELOW IN THIS FUNCTION
    SDL_HideWindow(window);

#ifdef USE_GLES
    SDL_GetWindowWMInfo(window, &wm_info);
#endif

#ifdef USE_GLES
    dispmanDisplay = vc_dispmanx_display_open(0);
#endif

#ifdef USE_GLES
    // Currently has no use with a desktop GL setup.
    // renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    sdl_window_surface = SDL_GetWindowSurface(window);
    background_surface = nullptr;
#endif

    try {
        init_gl();
    }
    catch (InitException e) {
#ifdef USE_GLES
        vc_dispmanx_display_close(dispmanDisplay);
#endif
        SDL_DestroyWindow (window);
        throw e;
    }

    // Temporary fix (which just seems to work) for a bug where focus
    // events are not generated for the first time focus is changed.
    // SEE ALSO ABOVE IN THIS FUNCTION.
    SDL_ShowWindow(window);

    windows[SDL_GetWindowID(window)] = this;

    LOG(INFO) << "Created GameWindow... " << std::endl;

}

GameWindow::~GameWindow() {
    LOG(INFO) << "Destructing GameWindow... " << std::endl;
    deinit_gl();
#ifdef USE_GLES
    vc_dispmanx_display_close(dispmanDisplay); // (???)
#endif
    windows.erase(SDL_GetWindowID(window));

    //delete curGameInit;

    delete app;
    //delete mainWin;


    callback_controller.disable();

    delete input_manager;

    LOG(INFO) << "Destructed GameWindow... " << std::endl;

}

void GameWindow::init_sdl() {
    int result;

#ifdef USE_GLES
    bcm_host_init();
#ifndef STATIC_OVERSCAN
    std::tie(overscan_left, overscan_top) = query_overscan(overscan_left, overscan_top);
#endif
#endif

    LOG(INFO) << "Initializing SDL...";
    result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    if (result != 0) {
        throw GameWindow::InitException("Failed to initialize SDL");
        LOG(INFO) << "failed to init SDL\n";
    }

#ifdef USE_GLES
    SDL_VERSION(&wm_info.version);
#endif

    LOG(INFO) << "SDL initialized.";
}


void GameWindow::deinit_sdl() {
    LOG(INFO) << "Deinitializing SDL...";

    // Should always work.
    SDL_Quit ();

    LOG(INFO) << "SDL deinitialized.";
}


void GameWindow::init_gl() {
#ifdef USE_GLES
    EGLBoolean result;

    static const EGLint attribute_list[] = {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 0,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT | EGL_PBUFFER_BIT,
        EGL_NONE
    };

    static const EGLint context_attributes[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    // Get an EGL display connection

    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        throw GameWindow::InitException("Error getting display");
    }

    // Initialize EGL display connection
    result = eglInitialize(display, nullptr, nullptr);
    if (result == EGL_FALSE) {
        eglTerminate(display);
        throw GameWindow::InitException("Error initializing display connection");
    }

    // Get frame buffer configuration.
    result = eglChooseConfig(display, attribute_list, &config, 1, &configCount);
    if (result == EGL_FALSE) {
        eglTerminate(display);
        throw GameWindow::InitException("Error getting window frame buffer configuration");
    }

    //Should I use eglBindAPI? It is auomatically ES anyway.

    // Create EGL rendering context
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attributes);
    if (context == EGL_NO_CONTEXT) {
        eglTerminate(display);
        throw GameWindow::InitException("Error creating rendering context");
    }

    // Surface initialization is done here as it can be called multiple
    // times after main initialization.
    init_surface();
#endif
#ifdef USE_GL
    sdl_gl_context = SDL_GL_CreateContext(window);
#endif
}


void GameWindow::deinit_gl() {
#ifdef USE_GLES
    // Release EGL resources
    deinit_surface();
    eglDestroyContext(display, context);
    eglTerminate(display);
#endif
#ifdef USE_GL
    SDL_GL_DeleteContext(sdl_gl_context);
#endif
}


void GameWindow::init_surface() {
    int x, y, w, h;

#ifdef USE_GLES
    // It turns out that SDL's window position information is not good
    // enough, as it reports for the window border, not the rendering
    // area. For the time being, we shall be using LibX11 to query the
    // window's position.

    // child is just a place to put something. We don't need it.
    Window child;
    XTranslateCoordinates(wm_info.info.x11.display,
                          wm_info.info.x11.window,
                          XDefaultRootWindow(wm_info.info.x11.display),
                          0,
                          0,
                          &x,
                          &y,
                          &child);
#endif
    // SDL_GetWindowPosition(window, &x, &y);
    //SDL_GetWindowSize(window, &w, &h);
    //w = curGameInit->getGameWidth();
    //h = curGameInit->getGameHeight();
    w = mainWin->getGameWidgetWidth();
    h = mainWin->getGameWidgetHeight();
#ifdef USE_GL
    // We don't care in desktop GL.
    x = y = 0;
#endif
    init_surface(x, y, w, h);
}

int GameWindow::get_game_window_height(){
    return mainWin->getGameWidgetHeight();
}

int GameWindow::get_game_window_width(){
    return mainWin->getGameWidgetWidth();
}

void GameWindow::init_surface(int x, int y, int w, int h) {
    deinit_surface();
    // Because deinit clears this.
    change_surface = InitAction::DO_INIT;
#ifdef USE_GLES
    EGLSurface new_surface;

    EGLBoolean result;

    if (foreground) {
        // Rendering directly to screen.

        VC_RECT_T destination;
        VC_RECT_T source;

        static EGL_DISPMANX_WINDOW_T nativeWindow;

        VLOG(2) << "Initializing window surface.";

        // Create EGL window surface.

        destination.x = x + GameWindow::overscan_left;
        destination.y = y + GameWindow::overscan_top;
        destination.width = w;
        destination.height = h;

        VLOG(2) << "New surface: " << w << "x" << h << " at (" << x << "," << y << ").";

        source.x = 0;
        source.y = 0;
        source.width  = w << 16; // (???)
        source.height = h << 16; // (???)

        DISPMANX_UPDATE_HANDLE_T dispmanUpdate;
        dispmanUpdate  = vc_dispmanx_update_start(0); // (???)

        dispmanElement = vc_dispmanx_element_add(dispmanUpdate, dispmanDisplay,
                                                 0/*layer*/, &destination, 0/*src*/,
                                                 &source, DISPMANX_PROTECTION_NONE,
                                                 0 /*alpha*/, 0/*clamp*/, (DISPMANX_TRANSFORM_T)0/*transform*/); // (???)

        nativeWindow.element = dispmanElement;
        nativeWindow.width = w; // (???)
        nativeWindow.height = h; // (???)
        vc_dispmanx_update_submit_sync(dispmanUpdate); // (???)

        new_surface = eglCreateWindowSurface(display, config, &nativeWindow, nullptr);
        if (new_surface == EGL_NO_SURFACE) {
            std::stringstream hex_error_code;
            hex_error_code << std::hex << eglGetError();

            throw GameWindow::InitException("Error creating window surface: " + hex_error_code.str());
        }
    } else {
        EGLint attribute_list[] = {
            EGL_WIDTH, w,
            EGL_HEIGHT, h,
            EGL_NONE
        };

        VLOG(2) << "New surface: " << w << "x" << h << " (Pixel Buffer).";

        new_surface = eglCreatePbufferSurface(display, config, attribute_list);
        if (new_surface == EGL_NO_SURFACE) {
            std::stringstream hex_error_code;
            hex_error_code << std::hex << eglGetError();

            throw GameWindow::InitException("Error creating pbuffer surface: " + hex_error_code.str());
        }

        sdl_window_surface = SDL_GetWindowSurface(window);

        // Create an SDL surface for background blitting. RGBX
        background_surface = SDL_CreateRGBSurface(0,
                                                  sdl_window_surface->w,
                                                  sdl_window_surface->h,
                                                  32,
#if SDL_BYTE_ORDER == SDL_BIG_ENDIAN
                                                  0xff000000,
                                                  0x00ff0000,
                                                  0x0000ff00,
                                                  0x00000000
#else
                                                  0x000000ff,
                                                  0x0000ff00,
                                                  0x00ff0000,
                                                  0x00000000
#endif
                                                  );
        SDL_SetSurfaceBlendMode(background_surface, SDL_BLENDMODE_NONE);
    }
    surface = new_surface;

    // Connect the context to the surface.
    result = eglMakeCurrent(display, surface, surface, context);
    if (result == EGL_FALSE) {
        throw GameWindow::InitException("Error connecting context to surface");
    }

    // Clean up any garbage in the SDL window.
    // SDL_RenderClear(renderer);
    // SDL_RenderPresent(renderer);
#endif

    was_foreground = foreground;
    visible = true;
    change_surface = InitAction::DO_NOTHING;
    // Only set these if the init was successful.
    window_x = x;
    window_y = y;
    window_width = w;
    window_height = h;
}


void GameWindow::deinit_surface() {
#ifdef USE_GLES
    if (visible) {
        int result;

        if (was_foreground) {
            DISPMANX_UPDATE_HANDLE_T dispmanUpdate;
            dispmanUpdate  = vc_dispmanx_update_start(0); // (???)
            vc_dispmanx_element_remove (dispmanUpdate, dispmanElement);
            vc_dispmanx_update_submit_sync(dispmanUpdate); // (???)
        } else {
            if (background_surface != nullptr) {
                SDL_FreeSurface(background_surface);
                background_surface = nullptr;
            }
        }

        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        result = eglDestroySurface(display, surface);
        if (result == EGL_FALSE) {
            throw GameWindow::InitException("Error destroying EGL surface");
        }
    }
    visible = false;
    change_surface = InitAction::DO_NOTHING;
#endif
}


void GameWindow::update() {

    SDL_Event event;
    bool close_all = false;

    for (auto pair : windows) {
        GameWindow* window = pair.second;
        window->input_manager->clean();
    }

    while (SDL_PollEvent(&event)) {
        GameWindow* window;
        switch (event.type) {
        case SDL_QUIT: // Primarily used for killing when we become blind.
            close_all = true;
            return;
            break;
        case SDL_WINDOWEVENT:
            window = windows[event.window.windowID];

            // Instead of reinitialising on every event, do it ater we have
            // scanned the event queue in full.
            // Should focus events be included?
            switch (event.window.event) {
            case SDL_WINDOWEVENT_CLOSE:
                window->request_close();
                break;
            case SDL_WINDOWEVENT_RESIZED:
                window->change_surface = InitAction::DO_INIT;
                window->resizing = true;
                break;
            case SDL_WINDOWEVENT_MAXIMIZED:
            case SDL_WINDOWEVENT_RESTORED:
            case SDL_WINDOWEVENT_MOVED:
            case SDL_WINDOWEVENT_SHOWN:
            case SDL_WINDOWEVENT_FOCUS_GAINED:
                VLOG(2) << "Need surface reinit (gained focus)";
#ifndef GAME_WINDOW_DISABLE_DIRECT_RENDER
                window->foreground = true;
#endif
                window->change_surface = InitAction::DO_INIT;
                focused_window = window;
                break;
            case SDL_WINDOWEVENT_FOCUS_LOST:
            case SDL_WINDOWEVENT_MINIMIZED:
            case SDL_WINDOWEVENT_HIDDEN:
                VLOG(2) << "Need surface reinit (lost focus)";
                window->foreground = false;
                // This used to deinit, but that is actually bad.
                window->change_surface = InitAction::DO_INIT;
                if (focused_window == window) {
                    focused_window = nullptr;
                }
                break;
            }
            break;
        }
        // Let the input manager use the event (even if we used it).
        if (focused_window) {
            focused_window->input_manager->handle_event(&event);
        }
    }

    // Perform updates to windows which may be required following
    // events, such as reinitializing surface, closing, callbacks.
    for (auto pair : windows) {
        GameWindow* window = pair.second;

#ifdef USE_GLES
        // Hacky fix: The events don't quite chronologically work, so
        // check the window position to start any needed surface update.
        int x, y;
        Window child;
        XTranslateCoordinates(window->wm_info.info.x11.display,
                              window->wm_info.info.x11.window,
                              XDefaultRootWindow(window->wm_info.info.x11.display),
                              0,
                              0,
                              &x,
                              &y,
                              &child);
        if ((window->window_x != x || window->window_y != y) && window->visible) {
            VLOG(2) << "Need surface reinit (moved).";
            window->change_surface = InitAction::DO_INIT;
        }
#endif

        switch (window->change_surface) {
        case InitAction::DO_INIT:
            try {
                window->init_surface();
            }
            catch (InitException e) {
                LOG(WARNING) << "Surface reinit failed: " << e.what();
            }
            break;
        case InitAction::DO_DEINIT:
            window->deinit_surface();
            break;
        case InitAction::DO_NOTHING:
            // Do nothing - hey, I don't like compiler warnings.
            break;
        }

        if (window->resizing) {
            window->resize_callbacks.broadcast(window);
            window->resizing = false;
        }
        window->input_manager->run_callbacks();

        if (close_all) {
            window->request_close();
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


std::pair<int, int> GameWindow::get_size() {
    return std::pair<int, int>(window_width, window_height);
}


void GameWindow::use_context() {
#ifdef USE_GLES
    if (visible) {
        eglMakeCurrent(display, surface, surface, context);
    }
    else {
        LOG(WARNING) << "Window surface is not visible. EGL context lost - OpenGL calls may fail!";
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }
#endif
#ifdef USE_GL
    SDL_GL_MakeCurrent(window, sdl_gl_context);
#endif
    GraphicsContext::current = &graphics_context;
}


GraphicsContext* GameWindow::get_context() {
    return &graphics_context;
}


void GameWindow::disable_context() {
#ifdef USE_GLES
    eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
#endif
#ifdef USE_GL
    SDL_GL_MakeCurrent(window, nullptr);
#endif
    GraphicsContext::current = nullptr;
}


void GameWindow::swap_buffers() {
#ifdef USE_GLES
    if (visible) {
        if (foreground) {
            // Only "direct"-to-screen rendering is double buffered.
            eglSwapBuffers(display, surface);
        } else {
            // Render the content of the pixel buffer to the SDL window.

            // Copy the render into a compatible surface.
            glReadPixels(0,
                         0,
                         background_surface->w,
                         background_surface->h,
                         GL_RGBA,
                         GL_UNSIGNED_BYTE,
                         background_surface->pixels);

            // Copy (blit) the surface (whilst flipping) to the SDL window's surface.
            SDL_Rect dst;
            SDL_Rect src;
            dst.x = src.x = 0;
            dst.w = src.w = sdl_window_surface->w;
            dst.h = src.h = 1;
            for (int y = 0; y < background_surface->h; y++) {
                src.y = sdl_window_surface->h - y - 1;
                dst.y = y;
                SDL_BlitSurface(background_surface, &src, sdl_window_surface, &dst);
            }
            SDL_UpdateWindowSurface(window);
        }
    }
#endif
#ifdef USE_GL
    SDL_GL_SwapWindow(window);
#endif
}


InputManager* GameWindow::get_input_manager() {
    return input_manager;
}

MainWindow* GameWindow::get_main_win(){
    return mainWin;
}

void GameWindow::execute_app(){
    app->exec();
}

void GameWindow::update_running(bool option){
    mainWin->setRunning(option);
}

void GameWindow::update_terminal_text(std::string text, bool error){
    mainWin->pushTerminalText(text,error);
}


std::pair<float,float> GameWindow::get_ratio_from_pixels(std::pair<int,int> pixels) {
    return std::pair<float,float>((float)pixels.first / (float)window_width, (float)pixels.second / (float) window_height);
}


void GameWindow::register_resize_handler(Callback<void, GameWindow*> callback) {
    resize_callbacks.register_callback(callback);
}

Lifeline GameWindow::register_resize_handler(std::function<void(GameWindow*)> func) {
    Callback<void, GameWindow*> callback(func);
    resize_callbacks.register_callback(callback);
    return Lifeline([this, callback] () {
            resize_callbacks.unregister_callback(callback);
        },
        callback_controller);
}

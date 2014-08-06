#ifndef GRAPHICS_CONTEXT_H
#define GRAPHICS_CONTEXT_H

extern "C" {
#ifdef USE_GL
#include <SDL2/SDL.h>
#endif

#ifdef USE_GLES
#include <EGL/egl.h>
#include <EGL/eglext.h>
#endif
}



#include "callback.hpp"
#include "callback_registry.hpp"



class GameWindow;



///
/// Used to unambiguously distinguish between and manage GL contexts.
///
/// The implementation is simply a wrapper around GameWindow which hides
/// the user from the internals of graphics management, whilst also
/// providing a system for resource clean up.
///
class GraphicsContext {
private:
    friend class GameWindow;

    ///
    /// The currently active context.
    ///
    /// This is nullptr when no context is active.
    ///
    static GraphicsContext* current;
    
    ///
    /// The window with the one-to-one mapping with the GL context.
    ///
    GameWindow* window;

    ///
    /// Meant only for creation in and for GameWindow.
    ///
    GraphicsContext(GameWindow* window);

    ///
    /// Will release resources which were registered to this context.
    ///
    ~GraphicsContext();

    ///
    /// Callback functions for cleaning up resources when the context
    /// is destroyed.
    ///
    CallbackRegistry<void> resource_releasers;
public:
    ///
    /// Return true if the contexts use the same GL context.
    ///
    /// Never used.
    ///
    bool operator==(GraphicsContext other);

    ///
    /// Start using this GL context.
    ///
    /// Note that GameWindow will set the current active context.
    ///
    void use();

    ///
    /// Register a resource releaser, which will run when the context
    /// is destroyed.
    ///
    void register_resource_releaser(Callback<void> callback);

    ///
    /// Get the current active context.
    ///
    /// @return The active context or nullptr when no context is active.
    ///
    static GraphicsContext* get_current();
};



#endif

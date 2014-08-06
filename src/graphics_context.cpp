#include <glog/logging.h>



#include "graphics_context.hpp"
#include "game_window.hpp"
#include "callback.hpp"
#include "callback_registry.hpp"



GraphicsContext* GraphicsContext::current = nullptr;



GraphicsContext::GraphicsContext(GameWindow* window):
    window(window) {
}


GraphicsContext::~GraphicsContext() {
    LOG(INFO) << "Graphics context " << this << " destroyed: Releasing resources.";
    resource_releasers.broadcast();
}


bool GraphicsContext::operator==(GraphicsContext other) {
    return window == other.window;
}


void GraphicsContext::use() {
    // window will set the current context.
    window->use_context();
}


void GraphicsContext::register_resource_releaser(Callback<void> callback) {
    LOG(INFO) << "Registered graphics resource releaser";
    resource_releasers.register_callback(callback);
}


GraphicsContext* GraphicsContext::get_current() {
    return current;
}

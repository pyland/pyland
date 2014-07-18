#include <iostream>
#include <functional>
#include <queue>
#include <set>

extern "C" {
#include <SDL2/SDL.h>
}

#include "input_manager.hpp"
#include "game_window.hpp"
#include "keyboard_input_event.hpp"
#include "lifeline.hpp"
#include "lifeline_controller.hpp"



#ifdef DEBUG
#define GAME_WINDOW_DEBUG
#endif



InputManager::InputManager(GameWindow* window):
    window(window),

    down_keys(std::set<int>()),
    pressed_keys(std::set<int>()),
    typed_keys(std::set<int>()),
    released_keys(std::set<int>()),

    mouse_x(0),
    mouse_y(0),
    
    mouse_start(0, 0, 0);
    mouse_from(0, 0, 0);
    mouse_to(0, 0, 0);
    
    key_events(std::queue<KeyboardInputEvent>()),
    mouse_events(std::queue<KeyboardInputEvent>()),

    callback_controller(LifelineController()) {
}


InputManager::~InputManager() {
    callback_controller.disable();
}


void InputManager::clean() {
    pressed_keys.clear();
    typed_keys.clear();
    released_keys.clear();

    pressed_buttons.clear();
    released_buttons.clear();
}


void InputManager::handle_event(SDL_Event* event) {
    switch (event->type) {
    case SDL_KEYDOWN:
        if (down_keys.count(event->key.keysym.scancode) == 0) {
            down_keys.insert(event->key.keysym.scancode);
            pressed_keys.insert(event->key.keysym.scancode);
            key_events.push(KeyboardInputEvent(this, event->key.keysym.scancode, true, true, true));
        }
        else {
            key_events.push(KeyboardInputEvent(this, event->key.keysym.scancode, true, false, true));
        }
        typed_keys.insert(event->key.keysym.scancode);
        break;
    case SDL_KEYUP:
        key_events.push(KeyboardInputEvent(this, event->key.keysym.scancode, false, true, false));
        down_keys.erase(event->key.keysym.scancode);
        released_keys.insert(event->key.keysym.scancode);
        break;
    case SDL_MOUSEBUTTONDOWN:
        down_buttons.insert(event->button.button);
        pressed_buttons.insert(event->button.button);
        mouse_from = mouse_to;
        mouse_to = MouseState(this, event->button.x, event->button.y, event->button.button);
        mouse_events.push(MouseInputEvent(mouse_start, mouse_from, mouse_to));
        mouse_start = mouse_to;
        break;
    case SDL_MOUSEBUTTONUP:
        down_buttons.erase(event->button.button);
        released_buttons.insert(event->button.button);
        mouse_from = mouse_to;
        mouse_to = MouseState(this, event->button.x, event->button.y, event->button.button);
        mouse_events.push(MouseInputEvent(mouse_start, mouse_from, mouse_to));
        mouse_start = mouse_to;
        break;
    case SDL_MOUSEMOTION:
        mouse_from = mouse_to;
        mouse_to = MouseState(this, event->motion.x, event->motion.y, event->motion.button);
        mouse_events.push(MouseInputEvent(mouse_start, mouse_from, mouse_to));
        break;
    }
}


void InputManager::run_callbacks() {
    while (!key_events.empty()) {
        KeyboardInputEvent& event = key_events.front();
        keyboard_callbacks.broadcast(event);
        if (event.down) {
            if (event.changed) {
                key_press_callbacks.broadcast(event);
            }
            key_down_callbacks.broadcast(event);
            if (event.typed) {
                key_type_callbacks.broadcast(event);
            }
        }
        else {
            key_release_callbacks.broadcast(event);
        }
        key_events.pop();
    }
    for (int key : down_keys) {
        // Don't duplicate events
        if (pressed_keys.count(key) == 0 && typed_keys.count(key) == 0) {
            KeyboardInputEvent event(this, key, true, false, false);
            keyboard_callbacks.broadcast(event);
            key_down_callbacks.broadcast(event);
        }
    }
}


bool InputManager::is_scan_down(int key) {
    return (down_keys.count(key) == 1);
}


bool InputManager::is_key_down(int key) {
    return (down_keys.count(SDL_GetScancodeFromKey(key)) == 1);
}


bool InputManager::is_scan_pressed(int key) {
    return (pressed_keys.count(key) == 1);
}


bool InputManager::is_key_pressed(int key) {
    return (pressed_keys.count(SDL_GetScancodeFromKey(key)) == 1);
}


bool InputManager::is_scan_released(int key) {
    return (released_keys.count(key) == 1);
}


bool InputManager::is_key_released(int key) {
    return (released_keys.count(SDL_GetScancodeFromKey(key)) == 1);
}


bool InputManager::is_mouse_down(int button) {
    return mouse_to.is_down(button);
}


bool InputManager::is_mouse_pressed(int button) {
    return mouse_to.is_down(button) && !mouse_from.is_down(button);
}


bool InputManager::is_mouse_released(int button) {
    return !mouse_to.is_down(button) && mouse_from.is_down(button);
}


std::pair<int,int> InputManager::get_mouse_pixels() {
    return std::pair<int,int>(mouse_to.x, mouse_to.y);
}


std::pair<float,float> InputManager::get_mouse_ratio() {
    return window->get_ratio_from_pixels(std::pair<int,int>(mouse_to.x, mouse_to.y));
}


void InputManager::register_keyboard_handler(Callback<void, KeyboardInputEvent> callback) {
    keyboard_callbacks.register_callback(callback);
}

Lifeline InputManager::register_keyboard_handler(std::function<void(KeyboardInputEvent)> func) {
    Callback<void, KeyboardInputEvent> callback(func);
    keyboard_callbacks.register_callback(callback);
    return Lifeline([this, callback] () {
            keyboard_callbacks.unregister_callback(callback);
        },
        callback_controller);
}


void InputManager::register_key_press_handler(Callback<void, KeyboardInputEvent> callback) {
    key_press_callbacks.register_callback(callback);
}

Lifeline InputManager::register_key_press_handler(std::function<void(KeyboardInputEvent)> func) {
    Callback<void, KeyboardInputEvent> callback(func);
    key_press_callbacks.register_callback(callback);
    return Lifeline([this, callback] () {
            key_press_callbacks.unregister_callback(callback);
        },
        callback_controller);
}


void InputManager::register_key_type_handler(Callback<void, KeyboardInputEvent> callback) {
    key_type_callbacks.register_callback(callback);
}

Lifeline InputManager::register_key_type_handler(std::function<void(KeyboardInputEvent)> func) {
    Callback<void, KeyboardInputEvent> callback(func);
    key_type_callbacks.register_callback(callback);
    return Lifeline([this, callback] () {
            key_type_callbacks.unregister_callback(callback);
        },
        callback_controller);
}


void InputManager::register_key_down_handler(Callback<void, KeyboardInputEvent> callback) {
    key_down_callbacks.register_callback(callback);
}

Lifeline InputManager::register_key_down_handler(std::function<void(KeyboardInputEvent)> func) {
    Callback<void, KeyboardInputEvent> callback(func);
    key_down_callbacks.register_callback(callback);
    return Lifeline([this, callback] () {
            key_down_callbacks.unregister_callback(callback);
        },
        callback_controller);
}


void InputManager::register_key_release_handler(Callback<void, KeyboardInputEvent> callback) {
    key_release_callbacks.register_callback(callback);
}

Lifeline InputManager::register_key_release_handler(std::function<void(KeyboardInputEvent)> func) {
    Callback<void, KeyboardInputEvent> callback(func);
    key_release_callbacks.register_callback(callback);
    return Lifeline([this, callback] () {
            key_release_callbacks.unregister_callback(callback);
        },
        callback_controller);
}


void InputManager::register_mouse_handler(Callback<void, MouseInputEvent> callback) {
    mouse_callbacks.register_callback(callback);
}

Lifeline InputManager::register_mouse_handler(std::function<void(MouseInputEvent)> func) {
    Callback<void, MouseInputEvent> callback(func);
    mouse_callbacks.register_callback(callback);
    return Lifeline([this, callback] () {
            mouse_callbacks.unregister_callback(callback);
        },
        callback_controller);
}

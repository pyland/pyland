#include <iostream>
#include <set>
#include <functional>

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
    released_keys(std::set<int>()),

    mouse_x(0),
    mouse_y(0),
    
    down_buttons(std::set<int>()),
    pressed_buttons(std::set<int>()),
    released_buttons(std::set<int>()),

    callback_controller(LifelineController()) {
}


InputManager::~InputManager() {
    callback_controller.disable();
}


void InputManager::clean() {
    pressed_keys.clear();
    released_keys.clear();
}


void InputManager::handle_event(SDL_Event* event) {
    switch (event->type) {
    case SDL_KEYDOWN:
        down_keys.insert(event->key.keysym.scancode);
        pressed_keys.insert(event->key.keysym.scancode);
        break;
    case SDL_KEYUP:
        down_keys.erase(event->key.keysym.scancode);
        released_keys.insert(event->key.keysym.scancode);
        break;
    case SDL_MOUSEBUTTONDOWN:
        down_buttons.insert(event->button.button);
        pressed_buttons.insert(event->button.button);
        break;
    case SDL_MOUSEBUTTONUP:
        down_buttons.erase(event->button.button);
        released_buttons.insert(event->button.button);
        break;
    case SDL_MOUSEMOTION:
        mouse_x = event->motion.x;
        mouse_y = event->motion.y;
        break;
    }
}


void InputManager::run_callbacks() {
    for (int key : pressed_keys) {
        keyboard_callbacks.broadcast(KeyboardInputEvent(this, key, true, true));
        key_press_callbacks.broadcast(KeyboardInputEvent(this, key, true, true));
    }
    for (int key : released_keys) {
        keyboard_callbacks.broadcast(KeyboardInputEvent(this, key, false, true));
        key_release_callbacks.broadcast(KeyboardInputEvent(this, key, false, true));
    }
    for (int key : down_keys) {
        keyboard_callbacks.broadcast(KeyboardInputEvent(this, key, true, false));
        key_down_callbacks.broadcast(KeyboardInputEvent(this, key, true, false));
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
    return (down_buttons.count(button) == 1);
}


bool InputManager::is_mouse_pressed(int button) {
    return (pressed_buttons.count(button) == 1);
}


bool InputManager::is_mouse_released(int button) {
    return (released_buttons.count(button) == 1);
}


std::pair<int,int> InputManager::get_mouse_pixels() {
    return std::pair<int,int>(mouse_x, mouse_y);
}


std::pair<float,float> InputManager::get_mouse_ratio() {
    return std::pair<float,float>((float)mouse_x, (float)mouse_y);
}


void InputManager::register_keyboard_handler(Callback<void, KeyboardInputEvent> callback) {
    keyboard_callbacks.register_callback(callback);
}

Lifeline InputManager::register_keyboard_handler(std::function<void(KeyboardInputEvent)> func) {
    Callback<void, KeyboardInputEvent> callback(func);
    keyboard_callbacks.register_callback(callback);
    // return Lifeline([callback] () {
    //         callback.unregister_everywhere();
    //     },
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

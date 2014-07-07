#include <iostream>
#include <set>

extern "C" {
#include <SDL2/SDL.h>
}

#include "input_manager.hpp"
#include "game_window.hpp"



#ifdef DEBUG
#define GAME_WINDOW_DEBUG
#endif



// Shared static member
// std::map<Uint32,GameWindow*> GameWindow::windows;



InputManager::InputManager(GameWindow* window) {
    down_keys = std::set<int>();
    pressed_keys = std::set<int>();
    released_keys = std::set<int>();
    
    down_buttons = std::set<int>();
    pressed_buttons = std::set<int>();
    released_buttons = std::set<int>();

    this->window = window;
}


InputManager::~InputManager() {
    // Do nothing special.
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

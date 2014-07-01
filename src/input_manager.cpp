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
    // NULL Just removes a compile warning.
    InputManager* manager = NULL;
    switch (event->type) {
    case SDL_KEYDOWN:
    case SDL_KEYUP:
        manager = GameWindow::windows[event->key.windowID]->input_manager;
        break;
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
        manager = GameWindow::windows[event->button.windowID]->input_manager;
    case SDL_MOUSEMOTION:
        manager = GameWindow::windows[event->motion.windowID]->input_manager;
        break;
    }
    
    switch (event->type) {
    case SDL_KEYDOWN:
        manager->down_keys.insert(event->key.keysym.scancode);
        manager->pressed_keys.insert(event->key.keysym.scancode);
        break;
    case SDL_KEYUP:
        manager->down_keys.erase(event->key.keysym.scancode);
        manager->released_keys.insert(event->key.keysym.scancode);
        break;
    case SDL_MOUSEBUTTONDOWN:
        manager->down_buttons.insert(event->button.button);
        manager->pressed_buttons.insert(event->button.button);
        break;
    case SDL_MOUSEBUTTONUP:
        manager->down_buttons.erase(event->button.button);
        manager->released_buttons.insert(event->button.button);
        break;
    case SDL_MOUSEMOTION:
        manager->mouse_x = event->motion.x;
        manager->mouse_y = event->motion.y;
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

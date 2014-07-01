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
    held_keys = std::set<int>();
    pressed_keys = std::set<int>();
    released_keys = std::set<int>();

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
    InputManager* manager;
    switch (event->type) {
    case SDL_KEYDOWN:
    case SDL_KEYUP:
        manager = GameWindow::windows[event->key.windowID]->input_manager;
        break;
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
        manager = GameWindow::windows[event->button.windowID]->input_manager;
        break;
    }
    
    switch (event->type) {
    case SDL_KEYDOWN:
        manager->held_keys.insert(event->key.keysym.scancode);
        manager->pressed_keys.insert(event->key.keysym.scancode);
        break;
    case SDL_KEYUP:
        manager->held_keys.erase(event->key.keysym.scancode);
        manager->released_keys.insert(event->key.keysym.scancode);
        break;
    }
}


bool InputManager::is_key_held(int key) {
    return (held_keys.count(key) == 1);
}


bool InputManager::is_char_held(int key) {
    return (held_keys.count(SDL_GetScancodeFromKey(key)) == 1);
}


bool InputManager::is_key_pressed(int key) {
    return (pressed_keys.count(key) == 1);
}


bool InputManager::is_char_pressed(int key) {
    return (pressed_keys.count(SDL_GetScancodeFromKey(key)) == 1);
}


bool InputManager::is_key_released(int key) {
    return (released_keys.count(key) == 1);
}


bool InputManager::is_char_released(int key) {
    return (released_keys.count(SDL_GetScancodeFromKey(key)) == 1);
}

// Input Manager to be expanded.



#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <set>

extern "C" {
#include <SDL.h>
}



///
/// Handles SDL input such that it can be accessed reliably and on
/// demand.
///
class InputManager {
private:
    ///
    /// Set of currently pressed keys (scancodes).
    ///
    /// Use this when the key's layout is important.
    ///
    std::set<SDL_Scancode> pressed_keys;
    ///
    /// Set of currently pressed keys (keycodes).
    ///
    /// Use this when the key's character is important.
    ///
    std::set<SDL_Keycode>  pressed_chars;

public:
    ///
    /// Prepares a new input manager.
    ///
    /// @param window The associated window to receive events from.
    ///
    InputManager(GameWindow* window);

    
    
    ~InputManager();
}

#endif

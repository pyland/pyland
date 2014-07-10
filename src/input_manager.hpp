#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <set>
#include <functional>

extern "C" {
#include <SDL2/SDL.h>
}

#include "callback.hpp"
#include "callback_registry.hpp"
#include "lifeline.hpp"
#include "lifeline_controller.hpp"



class GameWindow;
class InputEvent;
class KeyboardInputEvent;



///
/// Handles SDL input such that it can be accessed reliably and on
/// demand.
///
class InputManager {
private:
    friend class GameWindow;

    ///
    /// The associate GameWindow to handle events for.
    ///
    GameWindow* window;
    
    ///
    /// Set of currently depressed keys (scancodes).
    ///
    /// Use this when the key's layout is important.
    ///
    std::set<int> down_keys;
    ///
    /// Set of recently pressed keys (scancodes).
    ///
    std::set<int> pressed_keys;
    ///
    /// Set of recently released keys (scancodes).
    ///
    std::set<int> released_keys;

    ///
    /// The current x coordinate of the mouse.
    ///
    /// Stored in pixels from left of render-area.
    ///
    int mouse_x;
    ///
    /// The current y coordinate of the mouse.
    ///
    /// Stored in pixels from top of render-area.
    ///
    int mouse_y;

    ///
    /// Set of currently depressed mouse buttons.
    ///
    std::set<int> down_buttons;
    ///
    /// Set of recently pressed mouse buttons.
    ///
    std::set<int> pressed_buttons;
    ///
    /// Set of recently released mouse buttons.
    ///
    std::set<int> released_buttons;

    ///
    /// Keyboard callback registry.
    ///
    CallbackRegistry<void,KeyboardInputEvent> keyboard_callbacks;
    ///
    /// Key press callback registry.
    ///
    CallbackRegistry<void,KeyboardInputEvent> key_press_callbacks;
    ///
    /// Key down callback registry.
    ///
    CallbackRegistry<void,KeyboardInputEvent> key_down_callbacks;
    ///
    /// Key release callback registry.
    ///
    CallbackRegistry<void,KeyboardInputEvent> key_release_callbacks;

    ///
    /// Upon destruction, lifelines should be disabled.
    ///
    LifelineController callback_controller;

    ///
    /// Code to be run before handling event loops.
    ///
    /// Clears pressed and released keys and mouse buttons.
    ///
    void clean();
    
    ///
    /// Update the state using the given SDL_Event.
    ///
    /// Called by the friend class GameWindow's update static method.
    ///
    /// @param event An SDL_Event holding the event to process.
    ///
    void handle_event(SDL_Event* event);

    ///
    /// Run all callbacks, based upon current state information.
    ///
    void run_callbacks();

public:
    ///
    /// Prepares a new input manager.
    ///
    /// @param window The associated window to receive events from.
    ///
    InputManager(GameWindow* window);

    ~InputManager();

    ///
    /// Query whether a key is down, given scancode.
    ///
    /// This should be used for keys where layout is important.
    ///
    /// @param key The scancode of the key.
    ///
    bool is_scan_down(int key);
    ///
    /// Query whether a key is down, given keycode.
    ///
    /// This should be used for keys where symbol is important.
    ///
    /// @param key The keycode of the key.
    ///
    bool is_key_down(int key);
    ///
    /// Query whether a key was recently pressed, given scancode.
    ///
    /// @param key The scancode of the key.
    ///
    bool is_scan_pressed(int key);
    ///
    /// Query whether a key was recently pressed, given keycode.
    ///
    /// @param key The keycode of the key.
    ///
    bool is_key_pressed(int key);
    ///
    /// Query whether a key was recently released, given scancode.
    ///
    /// @param key The scancode of the key.
    ///
    bool is_scan_released(int key);
    ///
    /// Query whether a key was recently released, given keycode.
    ///
    /// @param key The keycode of the key.
    ///
    bool is_key_released(int key);
    
    ///
    /// Query whether a mouse button is down.
    ///
    /// @param button The button number.
    ///
    bool is_mouse_down(int button);
    ///
    /// Query whether a mouse button was recently pressed.
    ///
    /// @param button The button number.
    ///
    bool is_mouse_pressed(int button);
    ///
    /// Query whether a mouse button was recently released.
    ///
    /// @param button The button number.
    ///
    bool is_mouse_released(int button);

    ///
    /// Get the mouse cursor position in pixels from the top-left corner
    /// of the render-area.
    ///
    std::pair<int,int> get_mouse_pixels();

    ///
    /// Get the mouse cursor position as a ratio of the distances
    /// between the top-left corner and bottom-right corner.
    ///
    std::pair<float,float> get_mouse_ratio();

    ///
    /// Registers a callback function for keyboard input event handling.
    ///
    /// @param callback Callback to be used to handle an event.
    ///
    void register_keyboard_handler(Callback<void, KeyboardInputEvent> callback);
    ///
    /// Registers a callback function for keyboard input event handling.
    ///
    /// @param func Callback function to be used to handle an event.
    /// @return A lifeline which keeps the callback active.
    ///
    Lifeline register_keyboard_handler(std::function<void(KeyboardInputEvent)> func);

    ///
    /// Registers a callback function for key presses.
    ///
    /// @param callback Callback to be used to handle an event.
    ///
    void register_key_press_handler(Callback<void, KeyboardInputEvent> callback);
    ///
    /// Registers a callback function for key presses.
    ///
    /// @param func Callback function to be used to handle an event.
    /// @return A lifeline which keeps the callback active.
    ///
    Lifeline register_key_press_handler(std::function<void(KeyboardInputEvent)> func);

    ///
    /// Registers a callback function for key held down.
    ///
    /// @param callback Callback to be used to handle an event.
    ///
    void register_key_down_handler(Callback<void, KeyboardInputEvent> callback);
    ///
    /// Registers a callback function for key held down.
    ///
    /// @param func Callback function to be used to handle an event.
    /// @return A lifeline which keeps the callback active.
    ///
    Lifeline register_key_down_handler(std::function<void(KeyboardInputEvent)> func);

    ///
    /// Registers a callback function for key releases.
    ///
    /// @param callback Callback to be used to handle an event.
    ///
    void register_key_release_handler(Callback<void, KeyboardInputEvent> callback);
    ///
    /// Registers a callback function for key releases.
    ///
    /// @param func Callback function to be used to handle an event.
    /// @return A lifeline which keeps the callback active.
    ///
    Lifeline register_key_release_handler(std::function<void(KeyboardInputEvent)> func);
};

#endif

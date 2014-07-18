#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <functional>
#include <queue>
#include <set>

extern "C" {
#include <SDL2/SDL.h>
}

#include "callback.hpp"
#include "callback_registry.hpp"
#include "lifeline.hpp"
#include "lifeline_controller.hpp"
#include "keyboard_input_event.hpp"
#include "mouse_input_event.hpp"



class GameWindow;



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
    /// Set of recently repeated keys (scancodes).
    ///
    /// When you hold down a key, often the key will repeat (e.g. in a
    /// text editor).
    ///
    std::set<int> typed_keys;
    ///
    /// Set of recently released keys (scancodes).
    ///
    std::set<int> released_keys;

    ///
    /// The state of the mouse just before a mouse button changed state.
    ///
    MouseState mouse_start;
    ///
    /// The state of the mouse since the last event.
    ///
    MouseState mouse_from;
    ///
    /// The current state of the mouse.
    ///
    MouseState mouse_to;

    ///
    /// Queue of key events to be sent to callbacks.
    ///
    std::queue<KeyboardInputEvent> key_events;
    ///
    /// Queue of mouse events to be sent to callbacks.
    ///
    std::queue<MouseInputEvent> mouse_events;
    

    ///
    /// Keyboard callback registry.
    ///
    CallbackRegistry<void,KeyboardInputEvent> keyboard_callbacks;
    ///
    /// @deprecated
    /// Key press callback registry.
    ///
    CallbackRegistry<void,KeyboardInputEvent> key_press_callbacks;
    ///
    /// @deprecated
    /// Key type callback registry.
    ///
    /// Like key press, but also accepts auto-repeats.
    ///
    CallbackRegistry<void,KeyboardInputEvent> key_type_callbacks;
    ///
    /// @deprecated
    /// Key down callback registry.
    ///
    CallbackRegistry<void,KeyboardInputEvent> key_down_callbacks;
    ///
    /// @deprecated
    /// Key release callback registry.
    ///
    CallbackRegistry<void,KeyboardInputEvent> key_release_callbacks;
    ///
    /// Mouse callback registry.
    ///
    CallbackRegistry<void,MouseInputEvent> mouse_callbacks;

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
    /// Getter for the associated game window.
    ///
    GameWindow* get_window();

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
    /// Get the mouse cursor position in pixels from the bottom-left corner
    /// of the render-area.
    ///
    std::pair<int,int> get_mouse_pixels();

    ///
    /// Get the mouse cursor position as a ratio of the distances
    /// between the bottom-left corner and top-right corner.
    ///
    /// @return The x ratio is the first value, and the y value is the second.
    ///         The ratios for x and y are both floats from 0 to 1.
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
    /// @deprecated
    /// Registers a callback function for key presses.
    ///
    /// @param callback Callback to be used to handle an event.
    ///
    void register_key_press_handler(Callback<void, KeyboardInputEvent> callback);
    ///
    /// @deprecated
    /// Registers a callback function for key presses.
    ///
    /// @param func Callback function to be used to handle an event.
    /// @return A lifeline which keeps the callback active.
    ///
    Lifeline register_key_press_handler(std::function<void(KeyboardInputEvent)> func);

    ///
    /// @deprecated
    /// Registers a callback function for key typing.
    ///
    /// This triggers according to key repeating when a key is held.
    ///
    /// @param callback Callback to be used to handle an event.
    ///
    void register_key_type_handler(Callback<void, KeyboardInputEvent> callback);
    ///
    /// @deprecated
    /// Registers a callback function for key typing.
    ///
    /// This triggers according to key repeating when a key is held.
    ///
    /// @param func Callback function to be used to handle an event.
    /// @return A lifeline which keeps the callback active.
    ///
    Lifeline register_key_type_handler(std::function<void(KeyboardInputEvent)> func);

    ///
    /// @deprecated
    /// Registers a callback function for key held down.
    ///
    /// @param callback Callback to be used to handle an event.
    ///
    void register_key_down_handler(Callback<void, KeyboardInputEvent> callback);
    ///
    /// @deprecated
    /// Registers a callback function for key held down.
    ///
    /// @param func Callback function to be used to handle an event.
    /// @return A lifeline which keeps the callback active.
    ///
    Lifeline register_key_down_handler(std::function<void(KeyboardInputEvent)> func);

    ///
    /// @deprecated
    /// Registers a callback function for key releases.
    ///
    /// @param callback Callback to be used to handle an event.
    ///
    void register_key_release_handler(Callback<void, KeyboardInputEvent> callback);
    ///
    /// @deprecated
    /// Registers a callback function for key releases.
    ///
    /// @param func Callback function to be used to handle an event.
    /// @return A lifeline which keeps the callback active.
    ///
    Lifeline register_key_release_handler(std::function<void(KeyboardInputEvent)> func);
    
    ///
    /// Registers a callback function for mouse input event handling.
    ///
    /// @param callback Callback to be used to handle an event.
    ///
    void register_mouse_handler(Callback<void, MouseInputEvent> callback);
    ///
    /// Registers a callback function for mouse input event handling.
    ///
    /// @param func Callback function to be used to handle an event.
    /// @return A lifeline which keeps the callback active.
    ///
    Lifeline register_mouse_handler(std::function<void(MouseInputEvent)> func);
};

#endif

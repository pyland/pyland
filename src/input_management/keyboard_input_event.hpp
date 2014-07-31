#ifndef KEYBOARD_INPUT_EVENT_H
#define KEYBOARD_INPUT_EVENT_H

#include <functional>
#include <map>
#include <ostream>
#include <SDL2/SDL.h>
#include <set>

#include "input_event.hpp"



class InputManager;

///
/// Keyboard class for input event description.
///
struct KeyboardInputEvent : public InputEvent {
public:
    friend std::ostream& operator<<(std::ostream& os, const KeyboardInputEvent& event);

    KeyboardInputEvent(InputManager* manager, int scan_code, bool down, bool changed, bool typed);

    int scan_code;
    int key_code;

    ///
    /// Whether the key is down.
    ///
    bool down;

    ///
    /// Whether the key has changed between up or down.
    ///
    bool changed;

    ///
    /// If the key can be considered as a type-style key press.
    ///
    /// This will be true if the key gets pressed, or auto-repeat
    /// triggers an event. This should not be used to signal the input
    /// of text data.
    ///
    bool typed;
};



#endif

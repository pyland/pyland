#ifndef KEYBOARD_INPUT_EVENT_H
#define KEYBOARD_INPUT_EVENT_H



#include <set>

#include "SDL2/SDL.h"

#include "input_event.hpp"



class InputManager;

///
/// Keyboard class for input event description.
///
struct KeyboardInputEvent : public InputEvent {
public:
    KeyboardInputEvent(InputManager* manager, int scan_code, bool down, bool changed);
    int scan_code;
    int key_code;
    bool down;
    bool changed;
};



#endif

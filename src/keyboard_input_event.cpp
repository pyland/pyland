#include "keyboard_input_event.hpp"



KeyboardInputEvent::KeyboardInputEvent(InputManager* manager, int scan_code, bool down, bool changed):
    InputEvent(manager),
    scan_code(scan_code),
    key_code(key_code),
    down(down),
    changed(changed) {
}

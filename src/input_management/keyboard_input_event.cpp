#include <ostream>
#include <SDL2/SDL.h>

#include "input_event.hpp"
#include "keyboard_input_event.hpp"



KeyboardInputEvent::KeyboardInputEvent(InputManager* manager, int scan_code, bool down, bool changed, bool typed):
    InputEvent(manager),
    scan_code(scan_code),
    key_code(SDL_GetKeyFromScancode((SDL_Scancode)scan_code)),
    down(down),
    changed(changed),
    typed(typed) {
}

std::ostream& operator<<(std::ostream& stream, const KeyboardInputEvent& event) {
    return stream << "KeyboardInputEvent(scan_code=" << event.scan_code
                                   << ", key_code="  << event.key_code
                                   << ", down="      << event.down
                                   << ", changed="   << event.changed
                                   << ", typed="     << event.typed << ")";
}

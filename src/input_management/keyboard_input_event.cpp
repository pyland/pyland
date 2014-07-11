#include <SDL2/SDL.h>

#include "keyboard_input_event.hpp"



KeyboardInputEvent::KeyboardInputEvent(InputManager* manager, int scan_code, bool down, bool changed):
    InputEvent(manager),
    scan_code(scan_code),
    key_code(SDL_GetKeyFromScancode((SDL_Scancode)scan_code)),
    down(down),
    changed(changed) {
}

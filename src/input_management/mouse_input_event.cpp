#include <SDL2/SDL.h>

#include "input_manager.hpp"
#include "mouse_input_event.hpp"



MouseInputEvent::MouseInputEvent(InputManager* manager,
                                 MouseState start,
                                 MouseState from,
                                 MouseState to):
    InputEvent(manager),
    start(start),
    from(from),
    to(to) {
}

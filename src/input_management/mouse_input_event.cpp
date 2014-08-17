#include "input_event.hpp"
#include "mouse_input_event.hpp"
#include "mouse_state.hpp"

class InputManager;


MouseInputEvent::MouseInputEvent(InputManager* manager,
                                 MouseState start,
                                 MouseState from,
                                 MouseState to,
                                 int button):
    InputEvent(manager),
    start(start),
    from(from),
    to(to),
    button(button),
    was_down(button == 0 ? false : (((from.buttons >> (button - 1)) & 1) != 0)),
    down(button == 0 ? false : (((to.buttons >> (button - 1)) & 1) != 0)) {
}

std::ostream& operator<<(std::ostream& stream, const MouseInputEvent& event) {
    return stream << "MouseInputEvent(start="     << event.start
                                << ", from="      << event.from
                                << ", to="        << event.to
                                << ", button="    << event.button << ")";
}

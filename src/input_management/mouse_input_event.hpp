#ifndef MOUSE_INPUT_EVENT_H
#define MOUSE_INPUT_EVENT_H

#include <ostream>

#include "input_event.hpp"
#include "mouse_state.hpp"


class MouseInputEvent : public InputEvent {
public:
    friend std::ostream& operator<<(std::ostream& os, const MouseInputEvent& event);

    ///
    /// The state just before a mouse button changed state.
    ///
    /// This keeps its state during the a state transition, so that
    /// dragging start and end can be easily identified.
    /// It keeps the button state of before the state transition.
    ///
    MouseState start;
    ///
    /// The state since the last event.
    ///
    /// If from.buttons != to.buttons then the state has just changed.
    ///
    MouseState from;
    ///
    /// The current state.
    ///
    MouseState to;
    ///
    /// The button which is the focus of interest (if any).
    ///
    /// One of Button.
    ///
    int button;
    ///
    /// Whether the button referred to by button was down previously.
    ///
    bool was_down;
    ///
    /// Whether the button referred to by button is down.
    ///
    bool down;

    MouseInputEvent(InputManager* manager,
                    MouseState start,
                    MouseState from,
                    MouseState to,
                    int button = 0);
};



#endif

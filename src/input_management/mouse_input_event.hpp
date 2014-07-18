#ifndef MOUSE_INPUT_EVENT_H
#define MOUSE_INPUT_EVENT_H

#include "input_event.hpp"
#include "mouse_state.hpp"



public class MouseInputEvent : public InputEvent {
public:
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
};



#endif

#ifndef MOUSE_STATE_H
#define MOUSE_STATE_H



public class MouseState {
public:
    ///
    /// Enumeration for the different button numbers.
    ///
    enum class Button {
        LEFT = 1,
        MIDDLE = 2,
        RIGHT = 3,
    };
    enum class ButtonMask {
        LEFT = 1 << Button::LEFT,
        MIDDLE = 1 << Button::MIDDLE,
        RIGHT = 1 << Button::RIGHT,
    };
    ///
    /// The horizontal position of the cursor in pixels from the left
    /// edge.
    ///
    int x;
    ///
    /// The vertical position of the cursor in pixels from the bottom
    /// edge.
    ///
    int y;
    ///
    /// The button state of the mouse.
    ///
    /// Individual bits represent a button.
    ///
    int buttons;

    MouseState(int x, int y, ButtonMask buttons);
};



#endif

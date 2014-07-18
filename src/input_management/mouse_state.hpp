#ifndef MOUSE_STATE_H
#define MOUSE_STATE_H



class MouseState {
public:
    ///
    /// Pseudo-enumeration for the different button numbers.
    ///
    class Button {
    public:
        static const int LEFT   = 1;
        static const int MIDDLE = 2;
        static const int RIGHT  = 3;
    };
    class ButtonMask {
    public:
        static const int NONE   = 0x0;
        static const int LEFT   = 0x1;
        static const int MIDDLE = 0x2;
        static const int RIGHT  = 0x4;
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

    MouseState(int x, int y, int buttons);

    bool is_down(int button);
};



#endif

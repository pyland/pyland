#ifndef INPUT_EVENT_H
#define INPUT_EVENT_H



class InputManager;

///
/// Super class for input event description.
///
struct InputEvent {
public:
    InputManager* manager;
    InputEvent(InputManager* manager);
};


#endif

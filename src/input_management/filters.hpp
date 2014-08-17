#ifndef FILTERS_H
#define FILTERS_H

#include <functional>
#include <initializer_list>
#include <string>

struct MouseInputEvent;
struct KeyboardInputEvent;


using KeyboardFilter  = std::function<bool (KeyboardInputEvent)>;
using KeyboardHandler = std::function<void (KeyboardInputEvent)>;

using MouseFilter  = std::function<bool (MouseInputEvent)>;
using MouseHandler = std::function<void (MouseInputEvent)>;


KeyboardHandler filter(std::initializer_list<KeyboardFilter> filters, KeyboardHandler wrapped);
MouseHandler filter(std::initializer_list<MouseFilter> filters, MouseHandler wrapped);


KeyboardFilter KEY(std::string key);
KeyboardFilter KEY(std::initializer_list<std::string> keys);

KeyboardFilter MODIFIER(std::string modifier);
KeyboardFilter MODIFIER(std::initializer_list<std::string> modifiers);


MouseFilter MOUSE_BUTTON(std::string button);
MouseFilter MOUSE_BUTTON(std::initializer_list<std::string> button);
MouseFilter MOUSE_BUTTON(int button);
MouseFilter MOUSE_BUTTON(std::initializer_list<int> button);


KeyboardFilter REJECT(KeyboardFilter filter);
MouseFilter REJECT(MouseFilter filter);

KeyboardFilter ANY_OF(std::initializer_list<KeyboardFilter> filters);
MouseFilter ANY_OF(std::initializer_list<MouseFilter> filters);



extern KeyboardFilter KEY_HELD;
extern KeyboardFilter KEY_PRESS;
extern KeyboardFilter KEY_REPEAT;
extern KeyboardFilter KEY_RELEASE;


///
/// Any mouse buttons are down.
///
extern MouseFilter MOUSE_HELD;
///
/// Any mouse buttons have just gone down.
///
extern MouseFilter MOUSE_PRESS;
///
/// Any mouse buttons have just gone up.
///
extern MouseFilter MOUSE_RELEASE;
///
/// The mouse has recently moved.
///
extern MouseFilter MOUSE_MOVE;
///
/// The mouse has gone up (all buttons) and has moved since a button
/// has gone down.
///
extern MouseFilter MOUSE_DRAG;
///
/// The mouse has gone up (all buttons) and did not moved while any
/// buttons were down.
///
extern MouseFilter MOUSE_CLICKED;



#endif

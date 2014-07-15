#ifndef FILTERS_H
#define FILTERS_H

#include <algorithm>
#include <functional>
#include <initializer_list>
#include <SDL2/SDL.h>

#include "input_manager.hpp"
#include "keyboard_input_event.hpp"

typedef std::function<bool (KeyboardInputEvent)> KeyboardFilter;
typedef std::function<void (KeyboardInputEvent)> KeyboardHandler;

KeyboardHandler filter(std::initializer_list<KeyboardFilter> filters, KeyboardHandler wrapped);

KeyboardFilter KEY(char *key);
KeyboardFilter KEY(std::initializer_list<char *> keys);

KeyboardFilter MODIFIER(char *modifier);
KeyboardFilter MODIFIER(std::initializer_list<char *> modifiers);

KeyboardFilter REJECT(KeyboardFilter filter);
extern KeyboardFilter KEYHELD;
extern KeyboardFilter KEYPRESS;
extern KeyboardFilter KEYREPEAT;
extern KeyboardFilter KEYUP;

#endif

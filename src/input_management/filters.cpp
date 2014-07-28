#include <algorithm>
#include <functional>
#include <initializer_list>
#include <SDL2/SDL.h>
#include <vector>

#include "filters.hpp"
#include "input_manager.hpp"
#include "keyboard_input_event.hpp"



///
/// A template that wraps filters around any event.
///
template <class Event>
static std::function<void (Event)> filter_do(std::initializer_list<std::function<bool (Event)>> filters,
                                            std::function<void (Event)> wrapped) {
    // Initializer lists have stupid noncopy semantics on copy.
    // Move to a vector to prevent corruption.
    std::vector<std::function<bool (Event)>> filters_vector(filters); 

    return [filters_vector, wrapped] (Event event) {
        for (auto filter : filters_vector) {
            if (!filter(event)) {
                return;
            }
        }

        wrapped(event);
    };
}

// Wrap up the template function in nice overloaded functions.
KeyboardHandler filter(std::initializer_list<KeyboardFilter> filters, KeyboardHandler wrapped) {
    return filter_do<KeyboardInputEvent>(filters, wrapped);
}

MouseHandler filter(std::initializer_list<MouseFilter> filters, MouseHandler wrapped) {
    return filter_do<MouseInputEvent>(filters, wrapped);
}

// KeyboardHandler filter(std::initializer_list<KeyboardFilter> filters, KeyboardHandler wrapped) {
//     // Initializer lists have stupid noncopy semantics on copy.
//     // Move to a vector to prevent corruption.
//     std::vector<KeyboardFilter> filters_vector(filters); 

//     return [filters_vector, wrapped] (KeyboardInputEvent event) {
//         for (auto filter : filters_vector) {
//             if (!filter(event)) {
//                 return;
//             }
//         }

//         wrapped(event);
//     };
// }


KeyboardFilter KEY(std::initializer_list<std::string> keys) {
    std::set<SDL_Keycode> keyset;

    for (std::string key : keys) {
        SDL_Keycode keycode = SDL_GetKeyFromName(key.c_str());
        if (keycode == SDLK_UNKNOWN) {
            throw std::runtime_error("unrecognized key: " + key);
        }

        keyset.insert(keycode);
    }
    
    return [keyset] (KeyboardInputEvent event) {
        return keyset.find(event.key_code) != keyset.end();
    };
}

KeyboardFilter KEY(std::string key) {
    return KEY({key});
}


KeyboardFilter MODIFIER(std::initializer_list<std::string> modifiers) {
    std::set<SDL_Scancode> modifierset;

    for (std::string modifier : modifiers) {
        SDL_Scancode modifiercode = SDL_GetScancodeFromName(modifier.c_str());
        if (modifiercode == SDL_SCANCODE_UNKNOWN) {
            throw std::runtime_error("unrecognized modifier: " + modifier);
        }

        modifierset.insert(modifiercode);
    }
    
    return [modifierset] (KeyboardInputEvent event) {
        for (auto modifiercode : modifierset) {
            if (event.manager->is_scan_down(modifiercode)) {
                return true;
            }
        }
        return false;
    };
}

KeyboardFilter MODIFIER(std::string modifier) {
    return MODIFIER({modifier});
}



MouseFilter MOUSE_BUTTON(std::initializer_list<std::string> buttons) {
    int button_bits = 0;

    for (std::string button : buttons) {
        if (button == "left") {
            button_bits |= MouseState::ButtonMask::LEFT;
        }
        else if (button == "middle") {
            button_bits |= MouseState::ButtonMask::MIDDLE;
        }
        else if (button == "right") {
            button_bits |= MouseState::ButtonMask::RIGHT;
        }
    }
    
    return [button_bits] (MouseInputEvent event) {
        return ((event.from.buttons | event.to.buttons) & button_bits) != 0;
    };
}

MouseFilter MOUSE_BUTTON(std::string button) {
    return MOUSE_BUTTON({button});
}

MouseFilter MOUSE_BUTTON(std::initializer_list<int> buttons) {
    int button_bits = 0;

    for (int button : buttons) {
        button_bits |= (1 << button);
    }
    
    return [button_bits] (MouseInputEvent event) {
        return ((event.from.buttons | event.to.buttons) & button_bits) != 0;
    };
}

MouseFilter MOUSE_BUTTON(int button) {
    return MOUSE_BUTTON({button});
}


template<class Event>
static std::function<bool (Event)> REJECT_do(std::function<bool (Event)> filter) {
    return [filter] (Event event) {
        return !filter(event);
    };
}

KeyboardFilter REJECT(KeyboardFilter filter) {
    return REJECT_do<KeyboardInputEvent>(filter);
}

MouseFilter REJECT(MouseFilter filter) {
    return REJECT_do<MouseInputEvent>(filter);
}


template<class Event>
static std::function<bool (Event)> ANY_OF_do(std::initializer_list<std::function<bool (Event)>> filters) {
    // Initializer lists have stupid noncopy semantics on copy.
    // Move to a vector to prevent corruption.
    std::vector<std::function<bool (Event)>> filters_vector(filters); 

    return [filters_vector] (Event event) {
        for (auto filter : filters_vector) {
            if (filter(event)) {
                return true;
            }
        }

        return false;
    };
}

KeyboardFilter ANY_OF(std::initializer_list<KeyboardFilter> filters) {
    return ANY_OF_do<KeyboardInputEvent>(filters);
}

MouseFilter ANY_OF(std::initializer_list<MouseFilter> filters) {
    return ANY_OF_do<MouseInputEvent>(filters);
}


// KeyboardFilter REJECT(KeyboardFilter filter) {
//     return [filter] (KeyboardInputEvent event) {
//         return !filter(event);
//     };
// }

KeyboardFilter KEY_HELD = [] (KeyboardInputEvent event) {
    return event.down;
};

KeyboardFilter KEY_PRESS = [] (KeyboardInputEvent event) {
    return event.down && event.changed;
};

KeyboardFilter KEY_REPEAT = [] (KeyboardInputEvent event) {
    return event.down && !event.changed;
};

KeyboardFilter KEY_RELEASE = [] (KeyboardInputEvent event) {
    return !event.down && event.changed;
};



MouseFilter MOUSE_HELD = [] (MouseInputEvent event) {
    return event.to.buttons;
};

MouseFilter MOUSE_PRESS = [] (MouseInputEvent event) {
    return (event.to.buttons & ~event.from.buttons) != 0;
};

MouseFilter MOUSE_RELEASE = [] (MouseInputEvent event) {
    return (~event.to.buttons & event.from.buttons) != 0;
};

MouseFilter MOUSE_MOVE = [] (MouseInputEvent event) {
    return (event.to.x != event.from.x) || (event.to.y != event.from.y);
};

MouseFilter MOUSE_DRAG = [] (MouseInputEvent event) {
    return ((event.to.x != event.start.x) || (event.to.y != event.start.y)) && event.start.buttons != 0;
};

MouseFilter MOUSE_CLICKED = [] (MouseInputEvent event) {
    return ((event.to.x == event.start.x) && (event.to.y == event.start.y)) && event.start.buttons != 0;
};

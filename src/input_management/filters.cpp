#include <algorithm>
#include <functional>
#include <initializer_list>
#include <SDL2/SDL.h>
#include <vector>

#include "filters.hpp"
#include "input_manager.hpp"
#include "keyboard_input_event.hpp"
#include "print_debug.hpp"

KeyboardHandler filter(std::initializer_list<KeyboardFilter> filters, KeyboardHandler wrapped) {
    // Initializer lists have stupid noncopy semantics on copy.
    // Move to a vector to prevent corruption.
    std::vector<KeyboardFilter> filters_vector(filters); 

    return [filters_vector, wrapped] (KeyboardInputEvent event) {
        for (auto filter : filters_vector) {
            if (!filter(event)) {
                return;
            }
        }

        wrapped(event);
    };
}


KeyboardFilter KEY(std::initializer_list<char *> keys) {
    std::set<SDL_Keycode> keyset;

    for (char *key : keys) {
        SDL_Keycode keycode = SDL_GetKeyFromName(key);
        // TODO: Error checking

        keyset.insert(keycode);
    }
    
    return [keyset] (KeyboardInputEvent event) {
        return keyset.find(event.key_code) != keyset.end();
    };
}

// TODO: Consider efficiency
KeyboardFilter KEY(char *key) {
    return KEY({key});
}


KeyboardFilter MODIFIER(std::initializer_list<char *> modifiers) {
    std::set<SDL_Scancode> modifierset;

    for (char *modifier : modifiers) {
        SDL_Scancode modifiercode = SDL_GetScancodeFromName(modifier);
        // TODO: Error checking

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

// TODO: Consider efficiency
KeyboardFilter MODIFIER(char *modifier) {
    return MODIFIER({modifier});
}


KeyboardFilter REJECT(KeyboardFilter filter) {
    return [filter] (KeyboardInputEvent event) {
        return !filter(event);
    };
}

KeyboardFilter KEYHELD = [] (KeyboardInputEvent event) {
    return event.down;
};

KeyboardFilter KEYPRESS = [] (KeyboardInputEvent event) {
    return event.down && event.changed;
};

KeyboardFilter KEYREPEAT = [] (KeyboardInputEvent event) {
    return event.down && !event.changed;
};

KeyboardFilter KEYUP = [] (KeyboardInputEvent event) {
    return !event.down && event.changed;
};

#ifndef SPRITE_SWITCHER_H
#define SPRITE_SWITCHER_H

#include "button.hpp"

class SpriteSwitcher {
private:
    ///
    /// storing switcher buttons, useful for later
    ///
    std::vector<std::shared_ptr<Button>> switcher_buttons;
public:
    SpriteSwitcher();

    ///
    /// call this when adding a new sprite
    ///
    void add_sprite(int id);
};

#endif
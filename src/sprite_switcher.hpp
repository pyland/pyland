#ifndef SPRITE_SWITCHER_H
#define SPRITE_SWITCHER_H

#include <vector>

class SpriteSwitcher {
private:
    ///
    /// storing switcher buttons, useful for later
    ///
    std::vector<int> switcher_buttons;
public:
    SpriteSwitcher();
    ~SpriteSwitcher();

    ///
    /// call this when adding a new sprite
    ///
    void add_sprite(int id);
};

#endif

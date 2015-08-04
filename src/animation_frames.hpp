#ifndef ANIMATION_FRAMES_H
#define ANIMATION_FRAMES_H

#include <string>
#include <utility>

class AnimationFrames {
    public:
        AnimationFrames(std::string object_file_location, std::string sprite_file_location);

        std::pair<int, std::string> get_frame(); //TODO: Work out what get_frame actually did!

    private:
        std::string object_file_location; //The location of the object folder in the file-system relative to game. eg. characters/enemies/crocodile
        std::string sprite_file_location; //The location of the sprite relative to object_file_location/sprites eg. main if the sprite is sprites/main/0.png
};

#endif

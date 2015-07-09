#ifndef ANIMATION_FRAMES_H
#define ANIMATION_FRAMES_H

#include <string>
#include <utility>

class AnimationFrames {
    public:
        AnimationFrames(std::string object_file_location, std::string sprite_file_location);

        std::pair<int, std::string> get_frame(std::string section, float completion);
        std::pair<int, std::string> get_frame(std::string section);

    private:
        std::string object_file_location;
        std::string sprite_file_location;
};

#endif

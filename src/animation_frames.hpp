#ifndef ANIMATION_FRAMES_H
#define ANIMATION_FRAMES_H

#include <string>
#include <utility>

class AnimationFrames {
    public:
        AnimationFrames(std::string animation_frames_root);

        std::pair<int, std::string> get_frame(std::string section, float completion);
        std::pair<int, std::string> get_frame(std::string section);

    private:
        std::string animation_frames_root;
};

#endif
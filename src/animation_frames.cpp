#include <functional>
#include <iterator>
#include <map>
#include <string>
#include <tuple>
#include <utility>

#include "animation_frames.hpp"
#include "maptools.hpp"
#include "prettyprint.hpp"
#include "texture_atlas.hpp"

AnimationFrames::AnimationFrames(std::string object_file_location, std::string sprite_file_location):
    object_file_location(object_file_location),
    sprite_file_location(sprite_file_location) {
        VLOG(1) << "Frames rooted at " << object_file_location; //TODO: Make log clearer
}

std::pair<int, std::string> AnimationFrames::get_frame() {
    return std::make_pair(0, "../game/objects/" + object_file_location + "/sprites/" + sprite_file_location + "0.png"); //TODO: Make this use an ini file!!!!
}

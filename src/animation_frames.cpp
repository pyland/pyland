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

/*
std::pair<int, std::string> AnimationFrames::get_frame(std::string section, float completion) {
    std::map<std::string, std::string> names_to_tilesets(TextureAtlas::names_to_tilesets());
    auto begin(maptools::start_of(names_to_tilesets, object_file_location +"/"+ section));
    auto end  (maptools::end_of  (names_to_tilesets, object_file_location +"/"+ section));

    auto length(std::distance(begin, end));
    CHECK_NE(length, 0) << ": there are no DIRECTORIES matching the input " << "(" << object_file_location +"/"+ section << ").";

    auto animation_number(int(completion * float(length) * 2));
    animation_number -= 2 * length == animation_number;
    animation_number %= int(length);

    // Advance the iterator to the wanted texture
    for (int i=0; i < animation_number; ++i) { ++begin; }

    std::string tile_name, tileset_name;
    std::tie   (tile_name, tileset_name) = *begin;
    tile_name = object_file_location+"/"+ section +"/"+ tile_name;

    auto index(TextureAtlas::get_shared(tileset_name)->get_name_index(tile_name));
    return std::make_pair(index, tileset_name);
}
*/

std::pair<int, std::string> AnimationFrames::get_frame() {
    return std::make_pair(0, "../game/objects/" + object_file_location + "/sprites/" + sprite_file_location + "0.png");
}

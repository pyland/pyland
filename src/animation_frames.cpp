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

AnimationFrames::AnimationFrames(std::string animation_frames_root):
    animation_frames_root(animation_frames_root) {
        VLOG(1) << "Frames rooted at " << animation_frames_root;
}
std::pair<int, std::string> AnimationFrames::get_frame(std::string section, float completion) {
    std::map<std::string, std::string> names_to_tilesets(TextureAtlas::names_to_tilesets());
    auto begin(maptools::start_of(names_to_tilesets, animation_frames_root +"/"+ section));
    auto end  (maptools::end_of  (names_to_tilesets, animation_frames_root +"/"+ section));

    auto length(std::distance(begin, end));
    CHECK_NE(length, 0) << ": there are no DIRECTORIES matching the input " << "(" << animation_frames_root +"/"+ section << ").";

    auto animation_number(int(completion * float(length) * 2));
    animation_number -= 2 * length == animation_number;
    animation_number %= int(length);

    // Advance the iterator to the wanted texture
    for (int i=0; i < animation_number; ++i) { ++begin; }

    std::string tile_name, tileset_name;
    std::tie   (tile_name, tileset_name) = *begin;
    tile_name = animation_frames_root +"/"+ section +"/"+ tile_name;

    auto index(TextureAtlas::get_shared(tileset_name)->get_name_index(tile_name));
    return std::make_pair(index, tileset_name);
}

std::pair<int, std::string> AnimationFrames::get_frame(std::string section) {
    std::map<std::string, std::string> names_to_tilesets(TextureAtlas::names_to_tilesets());

    std::string tile_name(animation_frames_root + "/" + section);
    std::string tileset_name(names_to_tilesets.at(tile_name));
    auto index(TextureAtlas::get_shared(tileset_name)->get_name_index(tile_name)); // << TODO! error most likely not in get_shared but in get_name_index. Look at this tomorrow
    return std::make_pair(index, tileset_name);
}

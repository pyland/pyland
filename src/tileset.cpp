#include <map>
#include <memory>
#include <string>

#include "tileset.hpp"

#include "texture_atlas.hpp"



TileSet::TileSet(const std::string name, int width, int height, const std::string atlas):
    width(width),
    height(height),
    name(name),
    texture_atlas(TextureAtlas::get_shared(atlas))
{
}

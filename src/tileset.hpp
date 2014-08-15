#ifndef TILESET_H
#define TILESET_H

#include <map>
#include <memory>
#include <string>

#include "texture_atlas.hpp"



///
/// A Tileset used in the map
///
class TileSet {
    ///
    /// The width of the tileset in pixels
    ///
    int width = 0;

    ///
    /// The height of the tileset in pixels
    ///
    int height = 0;

    ///
    /// The source file for the tileset
    /// 
    std::string name = "";

    ///
    /// The texture atlas this tileset uses.
    ///
    std::shared_ptr<TextureAtlas> texture_atlas;

public:

    ///
    /// Construct a tileset
    ///
    TileSet(const std::string name, int width, int height, const std::string atlas);

    ///
    /// Get the width of the tileset in pixels
    /// 
    int get_width() { return width; }

    ///
    /// Set the width of the tileset in pixels
    /// 
    void set_width(int new_width) { width = new_width; }

    ///
    /// Get the height of the tileset in pixels
    ///
    int get_height() { return height; }

    ///
    /// Set the height of the tileset in pixels
    /// 
    void set_height(int new_height) { height = new_height; }

    ///
    /// Get the texture atlas.
    ///
    std::shared_ptr<TextureAtlas> get_atlas() { return texture_atlas; }
};

#endif

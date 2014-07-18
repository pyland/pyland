#ifndef TILESET_H
#define TILESET_H

#include <string>

///
/// A Tileset used in the map
///kjkjdsk
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
public:

    ///
    /// Construct a tileset
    ///
    TileSet(std::string _name, int _width, int _height) : width(_width), height(_height), name(_name) {}

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
        
};

#endif

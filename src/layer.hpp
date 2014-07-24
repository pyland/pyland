#ifndef LAYER_H
#define LAYER_H


#include <utility>
#include <memory>
#include <string>
#include <vector>

///
/// A layer on the map
///
class Layer {
    ///
    /// The width of the layer
    ///
    int width;

    ///
    /// The height of the layer 
    ///
    int height;

    ///
    /// The name of the layer
    ///
    std::string name;
    
    ///
    /// The layer data stored as tilesetname, tile id pair
    ///
    std::shared_ptr<std::vector<std::pair<std::string, int>>> layer;


public:
    ///
    /// Construct the new Layer
    ///
    Layer(int _width, int _height, std::string _name) : width(_width), height(_height), name(_name), layer() {}

    ///
    /// Add a tile to the layer
    /// 
    void add_tile(const std::string tileset, int tile_id);
    
    ///
    /// Get the id of the tile at the specified location
    /// @param x_pos the layer x offset
    /// @param y_pos the layer y offset
    ///
    int get_tile(int x_pos, int y_pos);

    ///
    /// Get the name of the layer
    ///
    std::string get_name() { return name; }

    ///
    /// Get the width of the layer 
    ///
    int get_width() { return width; }

    ///
    /// Get the height of the layer
    ///
    int get_height() { return height; }
    
    ///
    /// Get the layer's data as a shared_ptr
    ///
    std::shared_ptr<std::vector<std::pair<std::string, int>>> get_layer_data() { return layer; }
};

#endif

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
    /// The layer data stored as tilesetname, tile id pair
    ///
    std::shared_ptr<std::vector<std::pair<std::string, int>>> layer = std::make_shared<std::vector<std::pair<std::string, int>>>();


public:
    ///
    /// Construct the new Layer
    ///
    Layer(int _width, int _height) : width(_width), height(_height) {}

    ///
    /// Add a tile to the layer
    /// 
    void add_tile(const std::string tileset, int tile_id);

    ///
    /// Get the layer's data as a shared_ptr
    ///
    std::shared_ptr<std::vector<std::pair<std::string, int>>> get_layer_data() { return layer; }
};

#endif

#ifndef LAYER_H
#define LAYER_H

#include "object.hpp"

#include <utility>
#include <memory>
#include <string>
#include <vector>

///
/// A layer on the map. These
///
class Layer : public Object {

public:
    ///
    /// Enum class to indicate the packing of the layer data
    ///
    enum class Packing {
        ///
        /// We have a dense layer 
        ///
        DENSE,
        ///
        /// We have a sparse layer
        ///
       SPARSE
    };

private:
    ///
    /// The width of the layer in tiles
    ///
    int width_tiles;

    ///
    /// The height of the layer in tiles
    ///
    int height_tiles;

    ///
    /// The name of the layer
    ///
    std::string name;
    
    ///
    /// The layer data stored as tilesetname, tile id pair
    ///
    std::shared_ptr<std::vector<std::pair<std::string, int>>> layer;

    ///
    /// The packing of the layer
    ///
    Layer::Packing packing;

public:
    ///
    /// Construct the new Layer
    ///
    Layer(int _width_tiles, int _height_tiles, std::string _name);

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
    /// Gets the packing of the layer. This indicates how the data has been packed into
    /// the VBOs
    /// @return the packing
    ///
    Packing get_packing() { return packing; }

    ///
    /// Set the packing of the layer's data
    /// @param _packing specify how the data has been packed into
    /// the VBOs
    ///
    void set_packing(Packing _packing) { packing = _packing; }

    ///
    /// Get the name of the layer
    ///
    std::string get_name() { return name; }

    ///
    /// Get the width of the layer in tiles
    /// 
    int get_width_tiles() { return width_tiles; }

    ///
    /// Get the height of the layer
    ///
    int get_height_tiles() { return height_tiles; }
    
    ///
    /// Get the layer's data as a shared_ptr
    ///
    std::shared_ptr<std::vector<std::pair<std::string, int>>> get_layer_data() { return layer; }
};

#endif

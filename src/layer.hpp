#ifndef LAYER_H
#define LAYER_H

#include "object.hpp"

#include <exception>
#include <memory>
#include <string>
#include <utility>
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

    ///
    /// The map of tile locations to tile offset
    ///
    std::map<int, int> location_texture_vbo_offset_map;
public:
    ///
    /// Construct the new Layer
    ///
    Layer(int _width_tiles, int _height_tiles, std::string _name);

    ///
    /// Add a tile to the layer. This adds the tile to the end of the tile list.
    /// Note: this does NOT add the tile to the geometry. It adds it to the list of tiles on this layer.
    /// 
    void add_tile(const std::string tileset, int tile_id);

    ///
    /// Update a tile. Use THIS function to put a new tile on the layer or to update an  
    /// existing tile on the layer.
    /// @param x_pos the x position
    /// @param y_pos the y position
    /// @param tile_id the identifier of the tile
    /// @param tileset the name of the tileset for this tile
    /// Throws, LayerInvalidException if the x and y position is out of bounds
    ///
    void update_tile(int x_pos, int y_pos, int tile_id, std::string tileset);

    ///
    /// Get the id of the tile at the specified location
    /// @param x_pos the layer x offset
    /// @param y_pos the layer y offset
    ///
    int get_tile(int x_pos, int y_pos);

    ///
    /// Get a tile's texture offset in the VBO
    /// @return the texture offset in the VBo
    ///
    int get_tile_texture_vbo_offset();

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

///
/// The exception class for layers
///
class LayerInvalidException : public std::exception {
    virtual const char* what()  const throw() {
        return "Invalid operation on Layer";
    }
};
#endif



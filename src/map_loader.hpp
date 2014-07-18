#ifndef MAP_LOADER_HPP
#define MAP_LOADER_HPP

#include "layer.hpp"
#include "map_object.hpp"
#include "tileset.hpp"

#include <utility>
#include <memory>
#include <string>
#include <Tmx.h>
#include <vector>

///
/// Class to load a map from a TMX file
/// 
class MapLoader {
    ///
    /// The TMX map file
    ///
    Tmx::Map map;

    ///
    /// The width of the map
    ///
    int map_width = 0;

    ///
    /// The height of the map
    ///
    int map_height = 0;

    ///
    /// Load layers from the TMX map
    ///
    void load_layers();

    ///
    /// Load objects from the TMX map
    ///
    void load_objects();

    ///
    /// Load tilesets from the TMX map
    ///
    void load_tileset();

    ///
    /// Vector of tilesets
    ///
    std::vector<std::shared_ptr<TileSet>> tilesets;

    ///
    /// Array of layers
    ///
    std::vector<std::shared_ptr<Layer>> layers;
    
    ///
    /// Array of objects
    ///
    std::vector<std::shared_ptr<MapObject>> objects;    
 public:

    ///
    /// Load the TMX map from the source file
    /// 
    bool load_map(const std::string source);

    ///
    /// Gets the width of the ma
    /// @return the width of the map
    ///
    int get_map_width() { return map_width; }

    ///
    /// Get the height of the map
    /// @return the height of the map
    ///
    int get_map_height() { return map_height; }

    ///
    /// Get the tilesets that this map uses
    /// @return a vector of the tilesets, referenced by shared_ptrs
    ///
    std::vector<std::shared_ptr<TileSet>>& get_tilesets() {return tilesets; }

    ///
    /// Get the map's layers
    /// @return a vector of the layers, referenced by shared_ptrs
    ///
    std::vector<std::shared_ptr<Layer>>& get_layers() { return layers; }

    ///
    /// Get the objects in the map
    /// @return a vector of the objects in the map, referenced by shared_ptrs
    ///
    std::vector<std::shared_ptr<MapObject>>& get_objects() { return objects; }
};


#endif

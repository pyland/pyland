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
};


#endif

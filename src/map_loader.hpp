#ifndef MAP_LOADER_HPP
#define MAP_LOADER_HPP

#include <array>
#include <memory>
#include <string>
#include <Tmx.h>

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
    /// Array of layers
    ///
    std::array<smart_ptr<Layer>> layers;
    
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
        /// The layer data
        ///
        shared_ptr<std::array<int>> layer;
    public:
        Layer(int _width, int _height) : width(_width), height(_height) {}
        ~Layer();
    }
 public:

     ///
     /// Load the TMX map from the source file
    /// 
    bool load_map(const std::string source);

};


#endif

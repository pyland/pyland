#ifndef MAP_LOADER_HPP
#define MAP_LOADER_HPP


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
        std::vector<int> layer;
    public:
        Layer(int _width, int _height) : width(_width), height(_height) {}
    };

    ///
    /// An object in the map, different from an Object in the engine
    ///
    class MapObject {
        std::string name = "";
        int x_position = 0;
        int y_position = 0;

    public:
        std::string get_name() { return name; }
        void set_name(std::string _name) { name = _name; }
        
        int get_x_position() { return x_position; }
        void set_x_position(int x_pos) { x_position = x_pos; }
        
        int get_y_positioni() { return y_position; }
        void set_y_position(int y_pos) { y_position = y_pos; }
    };
    
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

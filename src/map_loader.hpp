#ifndef MAP_LOADER_HPP
#define MAP_LOADER_HPP

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

    ///
    /// An object in the map, different from an Object in the engine
    ///
    class MapObject {
        ///
        /// The name of the map object
        ///
        std::string name = "";

        ///
        /// The x position of the object
        ///
        int x_position = 0;

        ///
        /// The y position of the object
        /// 
        int y_position = 0;

    public:
        ///
        /// Get the name of the object
        ///
        std::string get_name() { return name; }

        ///
        /// Set the name of the object
        ///
        void set_name(std::string _name) { name = _name; }

        ///
        /// Get the x position of the object
        ///
        int get_x_position() { return x_position; }

        ///
        /// Set the x position of the object
        ///
        void set_x_position(int x_pos) { x_position = x_pos; }
        
        ///
        /// Get the y position of the object
        ///
        int get_y_positioni() { return y_position; }

        ///
        /// Set the y position of the object
        ///
        void set_y_position(int y_pos) { y_position = y_pos; }
    };

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

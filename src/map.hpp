#ifndef MAP_H
#define MAP_H

#include <map>
#include <memory>
#include <string>
#include <vector>

//Include GLM
#define GLM_FORCE_RADIANS
#include <glm/vec2.hpp>

#include "open_gl.hpp"

#include "dispatcher.hpp"
#include "fml.hpp"
#include "map_loader.hpp"

class Layer;
class TextureAtlas;
class TileSet;

class Map {
    ///
    /// Vector of tilesets
    ///
    std::vector<std::shared_ptr<TileSet>> tilesets;

    ///
    /// Array of layers. Layers are objects so they have ids
    ///
    std::vector<int> layer_ids;

    ///
    /// The id of the 'special' layer. Initialised during construction
    ///
    int special_layer_id = -1;

    ///
    /// A vector of layers which maps the (x, y) position in the map
    /// onto the offset in the buffer. The offsets are based off of
    /// the number of vertices and the number of dimensions. It gives
    /// the GLfloat offset.
    ///.This allows us to update the buffers to change the map.
    /// First param: layer number, starts at 0
    /// Second param: Map of (x, y) positions, flattened to a single
    ///               number, this is x+ y*map_width, which maps these
    ///               locations to their offset in the vertex and texture
    ///
    std::map<int, std::shared_ptr<std::map<int, int>>> layer_mappings;

    ///
    /// The ids of the objects that are on this map
    ///
    std::vector<int> object_ids;

    ///
    /// Cache of the tileset texture data for this Map
    ///
    GLfloat* tileset_tex_coords = nullptr;

    ///
    /// This is the height of the map in tiles
    ///
    int map_height;

    ///
    /// The width of the map in tiles
    ///
    int map_width;

    ///
    /// The texture atlases which store the map tile textures
    ///
    std::shared_ptr<TextureAtlas> texture_atlases[1];

    ///
    /// The number of dimensions in the vertex and texture buffers for a tile: (x, y) here
    ///
    int num_tile_dimensions = 2;

    ///
    /// The number of vertices for a tile
    ///
    int num_tile_vertices = 6;

    ///
    /// The function used to generate the cache of tile texture coordinates.
    ///
    void generate_tileset_coords(std::shared_ptr<TextureAtlas> texture);

    ///
    /// Generate the map's texture and vertex data
    ///
    void generate_data();

    ///
    /// Generates a layer's texture coordinates. Handles generating the
    /// data if the layers are sparse or dense. A dense layer is one
    /// with more tiles that are non-empty than empty ones.
    ///
    /// @data the array to put the data
    /// @data_size the size of he array in bytes
    /// @num_tiles the number of tiles to generate data for
    /// @layer the layer to generate the texture coordinates for
    /// @dense if the layer is dense or sparse
    ///
    void generate_layer_tex_coords(GLfloat* data, std::shared_ptr<Layer> layer, bool dense=true);

    ///
    /// Generates a layer's vertex coordinates. Handles generating the
    /// data if the layers are sparse or dense. A dense layer is one
    /// with more tiles that are non-empty than empty ones.
    ///
    /// @data the array to put the data
    /// @data_size the size of he array in bytes
    /// @num_tiles the number of tiles to generate data for
    /// @layer the layer to generate the texture coordinates for
    /// @dense if the layer is dense or sparse
    ///
    void generate_layer_vert_coords(GLfloat* data, std::shared_ptr<Layer> layer, bool dense=true);

    ///
    /// Calls genenerate_layer_vert_coords to generate a layer's vertex data
    ///
    /// @data the array to put the data
    /// @data_size the size of he array in bytes
    /// @num_tiles the number of tiles to generate data for
    /// @layer the layer to generate the texture coordinates for
    /// @dense if the layer is dense or sparse
    ///
    void generate_dense_layer_vert_coords(GLfloat* data, std::shared_ptr<Layer> layer);

    ///
    /// Calls generate_layer_tex_coords
    ///
    /// @data the array to put the data
    /// @data_size the size of he array in bytes
    /// @num_tiles the number of tiles to generate data for
    /// @layer the layer to generate the texture coordinates for
    /// @dense if the layer is dense or sparse
    ///
    void generate_dense_layer_tex_coords(GLfloat* data, std::shared_ptr<Layer> layer);

    ///
    /// Calls generate_layer_vert_coords
    ///
    /// @data the array to put the data
    /// @data_size the size of he array in bytes
    /// @num_tiles the number of tiles to generate data for
    /// @layer the layer to generate the texture coordinates for
    /// @dense if the layer is dense or sparse
    ///
    void generate_sparse_layer_vert_coords(GLfloat* data, std::shared_ptr<Layer> layer);

    ///
    /// Calls generate_layer_tex_coords
    ///
    /// @data the array to put the data
    /// @data_size the size of he array in bytes
    /// @num_tiles the number of tiles to generate data for
    /// @layer the layer to generate the texture coordinates for
    /// @dense if the layer is dense or sparse
    ///
    void generate_sparse_layer_tex_coords(GLfloat* data, std::shared_ptr<Layer> layer);

    ///
    /// Used when a tile needs to be added at a point. This function
    /// will recalculate the layer mappings as needed to ensure future
    /// updates update the correct buffer locations. Used for sparse maps.
    /// @param x_pos
    /// @param y_pos
    /// @param layer_num the layer
    /// @return true if the tile can just be overwritten,false if it needs to be inserted
    ///
    bool recalculate_layer_mappings(int x_pos, int y_pos, int layer_num);

    ///
    /// Initialises the textures
    ///
    void init_textures();

    ///
    /// Initialises this Map's shaders
    ///
    bool init_shaders();

public:
    Dispatcher<int> event_sprite_add;
    PositionDispatcher<int> event_step_on;
    PositionDispatcher<int> event_step_off;
    std::vector<std::vector<int>> blocker;

    Map(const std::string map_src);
    ~Map();

    ///
    /// Add a map object to the map
    /// @param object_id the id of the map object
    ///
    void add_map_object(int object_id);

    ///
    /// Remove a map object from the map
    /// @param object_id the id of the map object
    ///
    void remove_map_object(int object_id);

    ///
    /// FML-valid mapping from strings to object properties,
    /// for hierachically accessing locations on the map by name.
    ///
    std::map<std::string, MapObjectProperties> locations;

    ///
    /// Get the objects that are on this map
    /// @return a vector of sprite ids
    /// - we don't want class users to add sprites to the map
    /// using this vector
    ///
    const std::vector<int>& get_objects() { return object_ids; }

    ///
    /// Get the map width
    ///
    int get_width() { return map_width; }

    ///
    /// Get the map height
    ///
    int get_height() { return map_height; }

    ///
    /// Is this location walkable
    ///
    bool is_walkable(int x_pos, int y_pos);

    int get_tile_type(int x, int y);

    ///
    /// TODO: Heidi: Document this class
    /// Collision detection for generated elements
    ///
    class Blocker {
        public:
            Blocker(glm::ivec2 tile, std::vector <std::vector<int>>* blocker);
            ~Blocker();
            Blocker(const Map::Blocker &other);
            glm::ivec2 tile;
            std::vector <std::vector<int>>* blocker;
    };

    Blocker block_tile(glm::ivec2 tile);

    ///
    /// Get the layers on this map
    /// @return the layers
    ///
    std::vector<int> get_layers() { return layer_ids; }

    ///
    /// Update the tile at a given point in the map
    /// @param x_pos the x position of the tile
    /// @param y_pos the y position of the tile
    /// @param the the y position of the tile
    /// @param tile_name the global name of the tile
    ///
    void update_tile(int x_pos, int y_pos, const std::string layer_name, const std::string tile_name);

    ///
    /// Query the tile at a given point in the map.
    /// @param x_pos the x position of the tile.
    /// @param y_pos the y position of the tile.
    /// @return The tile's global name or, if empty, "".
    ///
    std::string query_tile(int x_pos, int y_pos, const std::string layer_name);


    ///
    /// Get a tile's texture offset in the VBO
    /// @return the texture offset in the VBo
    ///
    int get_tile_texture_vbo_offset(int layer_num, int x_pos, int y_pos);

};

#endif

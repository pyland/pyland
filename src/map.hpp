#ifndef MAP_H
#define MAP_H

#include <array>
#include <map>
#include <memory>
#include <string>
#include <vector>

//Include GLM
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef USE_GLES
#include <GLES2/gl2.h>
#endif

#ifdef USE_GL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif


#include "dispatcher.hpp"
#include "renderable_component.hpp"

class Layer;
class MapObject;
class TileSet;

class Map {
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
    /// The ids of the characters that are on this map
    ///
    std::vector<int> characters;

    ///
    /// Cache of the tileset texture data for this Map
    ///
    GLfloat* tileset_tex_coords = nullptr;
  
    ///
    /// The texture data for the tileset. This is the cached entries
    /// which allow generation of the map texture coordinates for a
    /// particular tile.
    ///
    GLfloat* tileset_tex_data = nullptr;

    ///
    /// This is the height of the map in tiles
    /// 
    int map_height = 16;

    ///
    /// The width of the map in tiles
    ///
    int map_width = 16;

    ///
    /// The speed at which we scroll the map.
    ///
    float map_scroll_speed = 32.0f;

    ///
    /// The far left  x position where we display the map
    ///
    float map_display_x = 0.0f;

    ///
    /// The lower y position we are currently displaying the map at.
    ///
    float map_display_y = 0.0f;

    ///
    /// The width of the map to be displayed on screen.
    ///
    float map_display_width = 30;

    ///
    /// The height of the map to be displayed on screen
    ///
    float map_display_height = 30;

    ///
    /// The texture buffer pointers
    ///
    Image texture_images[1];

    ///
    /// The tileset texture coordinates
    ///
    GLfloat* tileset_text_coords = nullptr;

    ///
    /// Pointer to the map vertex data
    ///
    GLfloat* map_data = nullptr;

    ///
    /// Pointer tno the map texture coordinate data
    ///
    GLfloat* map_tex_coords = nullptr;

    ///
    /// The function used to generate the cache of tile texture coordinates.
    /// 
    void generate_tileset_coords(Image* texture_image);

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
    void generate_layer_tex_coords(GLfloat* data, int data_size, int num_tiles, std::shared_ptr<Layer> layer, bool dense=true);

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
    void generate_layer_vert_coords(GLfloat* data, int data_size, int num_tiles, std::shared_ptr<Layer> layer, bool dense=true);

    ///
    /// Calls genenerate_layer_vert_coords to generate a layer's vertex data
    ///
    /// @data the array to put the data
    /// @data_size the size of he array in bytes
    /// @num_tiles the number of tiles to generate data for
    /// @layer the layer to generate the texture coordinates for
    /// @dense if the layer is dense or sparse 
    ///
    void generate_dense_layer_vert_coords(GLfloat* data, int data_size, int num_tiles, std::shared_ptr<Layer> layer);

    ///
    /// Calls generate_layer_tex_coords
    ///
    /// @data the array to put the data
    /// @data_size the size of he array in bytes
    /// @num_tiles the number of tiles to generate data for
    /// @layer the layer to generate the texture coordinates for
    /// @dense if the layer is dense or sparse 
    ///
    void generate_dense_layer_tex_coords(GLfloat* data, int data_size, int num_tiles, std::shared_ptr<Layer> layer);

    ///
    /// Calls generate_layer_vert_coords
    ///
    /// @data the array to put the data
    /// @data_size the size of he array in bytes
    /// @num_tiles the number of tiles to generate data for
    /// @layer the layer to generate the texture coordinates for
    /// @dense if the layer is dense or sparse 
    ///
    void generate_sparse_layer_vert_coords(GLfloat* data, int data_size, int num_tiles, std::shared_ptr<Layer> layer);

    ///
    /// Calls generate_layer_tex_coords
    ///
    /// @data the array to put the data
    /// @data_size the size of he array in bytes
    /// @num_tiles the number of tiles to generate data for
    /// @layer the layer to generate the texture coordinates for
    /// @dense if the layer is dense or sparse 
    ///
    void generate_sparse_layer_tex_coords(GLfloat* data, int data_size, int num_tiles, std::shared_ptr<Layer> layer);

    ///
    /// Used when a tile needs to be added at a point. This function
    /// will recalculate the layer mappings as needed to ensure future
    /// updates update the correct buffer locations. Used for sparse maps.
    /// @param x_pos
    /// @param y_pos
    /// @param layer_num the layer
    ///
    void recalculate_layer_mappings(int x_pos, int y_pos, int layer_num);

    ///
    /// Initialises the textures
    ///
    void init_textures();

    ///
    /// Initialises this Map's shaders
    ///
    bool init_shaders();

public: 

    Dispatcher<int> event_character_add;
    PositionDispatcher<int> event_step_on;
    PositionDispatcher<int> event_step_off;
    std::vector <std::vector<int>> blocker;

    Map(const std::string map_src);
    ~Map();

    ///
    /// Add a character to the map
    /// @param character_id the id of the character
    ///
    void add_character(int character_id);

    ///
    /// Remove a character from the map
    /// @param character_id the id of the character
    ///
    void remove_character(int character_id);

    ///
    /// Get the characters that are on this map 
    // Make this a copy
    ///
    const std::vector<int>& get_characters() { return characters; }
  
    ///
    /// Get the map width
    ///
    int get_width() { return map_width; }

    ///
    /// Get the map height
    /// 
    int get_height() { return map_height; }

    ///
    /// Get the display width of the map
    /// @return get the display width of the map
    ///
    float get_display_width() { return map_display_width; }

    ///
    /// Set the display width of the map
    /// @param display_width the new display width of the map
    ///
    void set_display_width(float display_width) { map_display_width = display_width; }

    ///
    /// Get the display height of the map
    /// @return get the display height of the map
    ///
    float get_display_height() { return map_display_height; }

    ///
    /// Set the display height of the map
    /// @param display_width the new display height of the map
    ///
    void set_display_height(float display_height) { map_display_height = display_height; }


    ///
    /// Get the map display lower left x position
    /// @return the map display far left x position
    ///
    float get_display_x() { return map_display_x; }

    ///
    /// Set the x display position of the map
    /// @param new_display_x the new display position
    ///
    void set_display_x(float new_display_x) { map_display_x = new_display_x; }
    
    ///
    /// Get the map display bottom y position
    /// @return the map display bottom y  position
    ///
    float get_display_y() { return map_display_y; }

    ///
    /// Set the y display position of the map
    /// @param new_display_y the new display position
    ///
    void set_display_y(float new_display_y) { map_display_y = new_display_y; }

    ///
    /// Is this location walkable
    /// 
    bool is_walkable(int x_pos, int y_pos);

    class Blocker {
        public:
            Blocker(Vec2D tile, std::vector <std::vector<int>>* blocker);
            ~Blocker();
            Blocker(const Map::Blocker &other);
            Vec2D tile;
            std::vector <std::vector<int>>* blocker;
    };

    Blocker block_tile(Vec2D tile);


    ///
    /// Get the layers on this map
    /// @return the layers
    ///
    std::vector<std::shared_ptr<Layer>> get_layers() { return layers; }

    ///
    /// Update the tile at a given point in the map
    /// @param x_pos the x position of the tile
    /// @param y_pos the y position of the tile
    /// @param tile_id the id of the tile
    /// @param tileset_name the tileset name, default tileset used if
    /// not specified
    ///
    void update_tile(int x_pos, int y_pos, int layer_num, int tile_id, std::string tileset_name="");
};

#endif

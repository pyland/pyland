#ifndef MAP_H
#define MAP_H

#include "character.hpp"
#include "game_window.hpp"
#include "layer.hpp"
#include "map_object.hpp"
#include "object.hpp"
#include "renderable_component.hpp"
#include "tileset.hpp"


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
    /// The ids of the characters that are on this map
    ///
    std::vector<int> characters;

    ///
    /// Cache of the tileset texture data for this Map
    ///
    GLfloat* tileset_tex_coords = nullptr;

    /// 
    /// Object instance to contain data needed to render the map
    ///
    RenderableComponent renderable_component;
  
    ///
    /// The vertex data for the map
    ///
    GLfloat* map_vertex_data = nullptr;

    /// 
    /// The texture data for the map
    ///
    GLfloat* map_tex_data = nullptr;
  
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
    int map_display_width = 30;

    ///
    /// The height of the map to be displayed on screen
    ///
    int map_display_height = 30;

    ///
    /// The texture buffer pointers
    ///
    char* tex_buf[1];

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
    void generate_tileset_coords(int tileset_width, int tileset_height);

    ///
    /// The function which generates the texture coordinates for the map
    /// geometry, using the cached tile coordinates.
    ///
    void generate_map_texcoords();
  
    ////
    /// The function which generates the map geometry so that it can be
    /// rendered to the screen
    ///
    void generate_map_coords();

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
    /// Gets the RenderableComponent object instance associated with this Map
    ///
    RenderableComponent* get_renderable_component() { return &renderable_component; }
  
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
    int get_display_width() { return map_display_width; }

    ///
    /// Set the display width of the map
    /// @param display_width the new display width of the map
    ///
    void set_display_width(int display_width) { map_display_width = display_width; }

    ///
    /// Get the display height of the map
    /// @return get the display height of the map
    ///
    int get_display_height() { return map_display_height; }

    ///
    /// Set the display height of the map
    /// @param display_width the new display height of the map
    ///
    void set_display_height(int display_height) { map_display_height = display_height; }


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
    /// The function used to update elements on the map.
    ///
    void update_map(float dt);

    ///
    /// Is this location walkable
    /// 
    bool is_walkable(int x_pos, int y_pos);

    class Blocker {
        public:
            Blocker(Vec2D tile, std::vector <std::vector<int>>* blocker);
            ~Blocker();
            Blocker(const Map::Blocker::Blocker &other);
            Vec2D tile;
            std::vector <std::vector<int>>* blocker;
    };

    Blocker block_tile (Vec2D tile);
};

#endif

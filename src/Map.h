#ifndef MAP_H
#define MAP_H

#include <string>

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

#include "game_window.hpp"
#include "RenderableComponent.h"


class Map {
  GLfloat* sprite_data = NULL;
  GLfloat* sprite_tex_data = NULL;
  GLfloat* tileset_tex_coords = NULL;

  /// 
  /// Object instance to contain data needed to render the map
  ///
  RenderableComponent renderable_component;
  

  ///
  /// Vertex Buffer Object Opengl identifiers
  ///
  GLuint vbo_objs[2];

  
  /// 
  /// The Opengl handle for the shader program currently being used
  ///
  GLuint program_obj = 0;

  /// 
  /// The Opengl handle for the tileset texture
  ///
  GLuint tileset_obj = 0;

  ///
  /// The vertex data for the map
  ///
  GLfloat* map_vertex_data = NULL;

  /// 
  /// The texture data for the map
  ///
  GLfloat* map_tex_data = NULL;
  
  ///
  /// The texture data for the tileset. This is the cached entries
  /// which allow generation of the map texture coordinates for a
  /// particular tile.
  ///
  GLfloat* tileset_tex_data = NULL;

  ///
  /// This is the height of the map in tiles
  /// 
  int map_height = 0;

  ///
  /// The width of the map in tiles
  ///
  int map_width = 0;

  ///
  /// The number of vertex buffer objects 
  ///
  static const int num_vbo_ids = 2;
 
  ///
  /// Array to hold the vertex buffer object Opengl Ids
  ///
  GLuint vbo_ids[num_vbo_ids] = { 0 };

  ///
  /// Array to hold the texture object identifiers
  ///
  GLuint texture_ids[2] = { 0 };

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
  int map_display_width = 8;

  ///
  /// The height of the map to be displayed on screen
  ///
  int map_display_height = 8;

  ///
  /// The texture buffer pointers
  ///
  char* tex_buf[2];

  ///
  /// The number of objects in the map
  ///
  int num_objects = 2;

  ///
  /// The world data
  ///
std::array<std::array<int, 16>, 16> world_data = {{
    {{14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14}},
    {{14,  64,  14,  64,  14,  64,  64,  64,  64,  64,  64,  64,  64,  64,  64,  14}},
    {{14,  64,  14,  64,  64,  64,  14,  64,  64,  64,  64,  64,  64,  64,  64,  14}},
    {{14,  64,  14,  64,  64,  64,  14,  64,  64,  64,  13,  13,  64,  64,  12,  14}},
    {{14,  64,  14,  64,  64,  14,  64,  64,  64,  64,  13,  13,  13,  64,  12,  14}},
    {{14,  64,  14,  64,  64,  14,  64,  64,  64,  13,  13,  13,  13,  13,  12,  14}},
    {{14,  64,  14,  64,  64,  14,  64,  64,  64,  13,  13,  13,  13,  13,  12,  14}},
    {{14,  64,  57,  64,  64,  14,  64,  64,  64,  64,  13,  14,  14,  14,  12,  14}},
    {{14,  64,  57,  64,  64,  14,  64,  64,  64,  64,  14,  74,   8,   8,   8,  14}},
    {{14,  64,  14,  64,  64,  14,  64,  64,  64,  64,  14,  74,   8,   8,   8,  14}},
    {{14,  64,  14,  64,  64,  14,  64,  64,  64,  14,  57,   8,   8,   8,   8,  14}},
    {{14,  64,  14,  64,  64,  14,  64,  64,  74,  14,   8,   8,   8,   8,   8,  14}},
    {{14,  64,  14,  64,  64,  14,  64,  74,  74,  14,   8,   8,   8,   8,   8,  14}},
    {{14,  64,  14,  14,  14,  14,  64,  74,  74,  14,   8,   8,   8,   8,   8,  14}},
    {{14,  64,  14,   8,   8,   8,  64,  74,  74,  14,   8,   8,   8,   8,   8,  14}},
    {{14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14}}
}};

  ///
  /// The tileset texture coordinates
  ///
  GLfloat* tileset_text_coords = NULL;

  ///
  /// Pointer to the map vertex data
  ///
  GLfloat* map_data = NULL;

  ///
  /// Pointer to the map texture coordinate data
  ///
  GLfloat* map_tex_coords = NULL;

  ///
  /// Function used to generate the necessary Vertex Buffer Objects to
  /// hold the map data to achieve more efficient rendering.
  ///
  void init_vbo_buffer();

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
  /// This function loads the shaders
  /// 
  GLuint load_shader(GLenum type, const std::string src);

  /// 
  /// This function crates the Opengl program
  ///
  GLuint shader_create(const std::string vs, const std::string fs);

  /// 
  /// This function loads the required texture images
  /// 
  void load_tex_images();
  
  /// 
  /// This function performs the required Opengl initializatin for the textures.
  ///
  void init_textures();

  ///
  /// This function initialises the shader, creating and loading them.
  /// 
  bool init_shaders();
  
public: 
  Map(const std::string map_src);
  ~Map();

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
  /// Get the map display lower left x position
  /// @return the map display far left x position
  ///
  float get_display_x() { return map_display_x; }

  ///
  /// Get the map display bottom y position
  /// @return the map display bottom y  position
  ///
  float get_display_y() { return map_display_y; }
  
  ///
  /// The function used to update elements on the map.
  ///
  void update_map(float dt);
};

#endif

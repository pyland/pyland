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



class Map {
  GLfloat* sprite_data;
  GLfloat* sprite_tex_data;
  GLfloat* tileset_tex_coords;

  ///
  /// Vertex Buffer Object Opengl identifiers
  ///
  GLuint vbo_objs[2];

  ///
  /// The reference to the window object in which we render the map.
  ///
  GameWindow *window;
  
  /// 
  /// The Opengl handle for the shader program currently being used
  ///
  GLuint program_obj;

  /// 
  /// The Opengl handle for the tileset texture
  ///
  GLuint tileset_obj;

  ///
  /// The vertex data for the map
  ///
  GLfloat* map_vertex_data;

  /// 
  /// The texture data for the map
  ///
  GLfloat* map_tex_data;
  
  ///
  /// The texture data for the tileset. This is the cached entries
  /// which allow generation of the map texture coordinates for a
  /// particular tile.
  ///
  GLfloat* tileset_tex_data;

  ///
  /// This is the height of the map in tiles
  /// 
  int map_height;

  ///
  /// The width of the map in tiles
  ///
  int map_width;
  
  ///
  /// The current projection matrix
  ///
  glm::mat4 projection_matrix;

  ///
  /// the current modelview matrix
  /// 
  glm::mat4 modelview_matrix;


  ///
  /// The number of vertex buffer objects 
  ///
  static const int num_vbo_ids = 4;
 
  ///
  /// Array to hold the vertex buffer object Opengl Ids
  ///
  GLuint vbo_ids[num_vbo_ids];

  ///
  /// Array to hold the texture object identifiers
  ///
  GLuint texture_ids[2];

  ///
  /// The speed at which we scroll the map.
  ///
  static const float map_scroll_speed;

  ///
  /// The far left  x position where we display the map
  ///
  float map_display_x;

  ///
  /// The lower y position we are currently displaying the map at.
  ///
  float map_display_y;

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
  GLfloat* tileset_text_coords;

  ///
  /// Pointer to the map vertex data
  ///
  GLfloat* map_data;

  ///
  /// Pointer to the map texture coordinate data
  ///
  GLfloat* map_tex_coords;

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
  Map();
  ~Map();

  ///
  /// The function used to render the map. Makes the necessary Opengl
  /// to correctly render the map.
  ///
  void render_map();

  ///
  /// The function used to update elements on the map.
  ///
  void update_map(float dt);
};

#endif

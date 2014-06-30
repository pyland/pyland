#ifndef MAP_H
#define MAP_H

#include "GLES/gl.h"
#include "GLES2/gl2.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"


deletclass Map {
  /**
   * Vertex Buffer Object Opengl identifiers
   */
  GLuint vbo_objs[2];

  /** 
   * The Opengl handle for the shader program currently being used
   */
  GLuint program_obj;

  /** 
   * The Opengl handle for the tileset texture
   */
  GLuint tileset_obj;

  /**
   * The vertex data for the map
   */
  GLfloat* map_vertex_data;

  /** 
   * The texture data for the map
   */
  GLfloat* map_tex_data;
  
  /**
   * The texture data for the tileset. This is the cached entries
   * which allow generation of the map texture coordinates for a
   * particular tile.
   */
  GLfloat* tileset_tex_data;

  /**
   * This is the height of the map in tiles
   */ 
  int map_height;

  /** 
   * The width of the map in tiles
   */
  int map_width;
  
  /**
   * The current projection matrix
   */
  glm::mat4 projection_matrix;

  /**
   * the current modelview matrix
   */ 
  glm::mat4 modelview_matrix;


  /**
   * Function used to generate the necessary Vertex Buffer Objects to
   * hold the map data to achieve more efficient rendering.
   */
  void init_vbo_buffer();

  /**
   * The function used to generate the cache of tile texture coordinates.
   */ 
  void generate_tileset_coords(int tileset_width, int tileset_height);

  /**
   * The function which generates the texture coordinates for the map
   * geometry, using the cached tile coordinates.
   */
  void generate_map_texcoords();
  
  /**
   * The function which generates the map geometry so that it can be
   * rendered to the screen
   */
  void generate_map_coords();

  /**
   * This function loads the shaders
   */ 
  GLuint load_shader(GLenum type, const std::string src);

  /** 
   * This function crates the Opengl program
   */
  GLuint shader_create(const string vs, const string fs);

  /** 
   * This function loads the required texture images
   */ 
  void load_tex_images();
  
  /** 
   * This function performs the required Opengl initializatin for the textures.
   */ 
  void init_textures();

  /**
   * This function initialises the shader, creating and loading them.
   */ 
  bool init_shaders();


  

public: 
  Map();
  ~Map();

  /**
   * The function used to render the map. Makes the necessary Opengl
   * to correctly render the map.
   */
  void render_map();

  /**
   * The function used to update elements on the map.
   */
  void update_map(float dt);
  
};

#endif

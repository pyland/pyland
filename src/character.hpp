#ifndef CHARACTER_H
#define CHARACTER_H

#include "object.hpp"

#ifdef USE_GLES

#include <GLES2/gl2.h>

#endif

#ifdef USE_GL

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#endif



class Character : public Object {
  GLfloat* sprite_tex_data = NULL;
  GLfloat* sprite_data = NULL;
  char* tex_buf = NULL;
  int tileset = 0;
 public:

  Character();
  ~Character();
  
  void generate_tex_data();
  void generate_vertex_data();
  void load_textures();
  bool init_shaders();
};

#endif

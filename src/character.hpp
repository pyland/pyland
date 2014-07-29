#ifndef CHARACTER_H
#define CHARACTER_H

#include "image.hpp"
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
    Image* texture_image = NULL;
    int tileset = 0;
public:

    Character();
    virtual ~Character();
  
    void generate_tex_data();
    void generate_vertex_data();
    void load_textures();
    bool init_shaders();
};

#endif

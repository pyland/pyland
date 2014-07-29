#ifndef CHARACTER_H
#define CHARACTER_H

#include "entitythread.hpp"
#include "object.hpp"

#ifdef USE_GLES

#include <GLES2/gl2.h>

#endif

#ifdef USE_GL

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#endif


///
/// Represents a character (sprite) in the engine
///
class Character : public Object {
    ///
    /// The texture coordinate data for the character
    ///
    GLfloat* sprite_tex_data = NULL;

    ///
    /// The vertex data for the character
    ///
    GLfloat* sprite_data = NULL;

    ///
    /// The texture (loaded image) data for the character
    ///
    char* tex_buf = NULL;

public:

    Character();
    virtual ~Character();

    ///
    /// Generate the texture coordinate data for the character
    ///
    void generate_tex_data();

    ///
    /// Generate the vertex data for the character
    ///
    void generate_vertex_data();

    ///
    /// Load the textures that are being used by the character
    ///
    void load_textures();

    ///
    /// Initialise the shaders that are being used by the character
    ///
    bool init_shaders();
};

#endif

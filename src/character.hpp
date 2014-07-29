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


///
/// Represents a character (sprite) in the engine
///
class Character : public Object {
    ///
    /// The texture coordinate data for the character
    ///
    GLfloat* sprite_tex_data = nullptr;

    ///
    /// The vertex data for the character
    ///
    GLfloat* sprite_data = nullptr;

    Image* texture_image = nullptr;

protected:

    ///
    /// Tiles that the object is blocking, probably
    /// by standing on.
    ///
    std::map<std::string, Map::Blocker> blocked_tiles;


    ///
    /// Utility function to make map replace less awful.
    ///
    void blocked_set(std::string key, Map::Blocker value);



public:

    Character();
    virtual ~Character();

    // TODO: Comment
    void set_state_on_moving_start(Vec2D target);
    void set_state_on_moving_finish();


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

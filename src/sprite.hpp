#ifndef SPRITE_H
#define SPRITE_H

#include "image.hpp"
#include "map_object.hpp"
#include "typeface.hpp"
#include "text_font.hpp"
#include "text.hpp"

#ifdef USE_GLES
#include <GLES2/gl2.h>
#endif

#ifdef USE_GL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif


///
/// Represents a sprite in the engine
///
class Sprite : public MapObject {

protected:

    ///
    /// Tiles that the object is blocking, probably
    /// by standing on.
    ///
    std::map<std::string, Map::Blocker> blocked_tiles;



public:

    Sprite(int _x_position, int _y_position, std::string _name);
    virtual ~Sprite();

    // TODO: Comment
    void set_state_on_moving_start(Vec2D target);
    void set_state_on_moving_finish();


    ///
    /// Generate the texture coordinate data for the sprite
    ///
    void generate_tex_data();

    ///
    /// Generate the vertex data for the sprite
    ///
    void generate_vertex_data();

    ///
    /// Load the textures that are being used by the sprite
    ///
    void load_textures();

    ///
    /// Initialise the shaders that are being used by the sprite
    ///
    bool init_shaders();
};

#endif

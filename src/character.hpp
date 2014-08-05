#ifndef CHARACTER_H
#define CHARACTER_H

#include "image.hpp"
#include "object.hpp"
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
/// Represents a character (sprite) in the engine
///
class Character : public Object {

    Text* character_text = nullptr;
    Text* status_text = nullptr;

protected:

    ///
    /// Tiles that the object is blocking, probably
    /// by standing on.
    ///
    std::map<std::string, Map::Blocker> blocked_tiles;



public:

    Character(int _x_position, int _y_position, std::string _name);
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

    Text* get_character_text() {return character_text; }
    void set_character_text(Text* _character_text) {character_text = _character_text; }
    Text* get_status_text() {return status_text; }
    void set_status_text(Text* _status_text) {status_text = _status_text; }

};

#endif

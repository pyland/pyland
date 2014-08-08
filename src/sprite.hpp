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
    /// The name of the spritesheet to use for the map object
    ///
    std::string sprite_sheet;

    ///
    /// The id of the sprite in the sheet
    ///
    int sprite_sheet_id;

    ///
    /// The text to display above the object
    ///
    Text* object_text = nullptr;

    ///
    /// The status text for the object
    ///
    Text* status_text = nullptr;


    ///
    /// Tiles that the object is blocking, probably
    /// by standing on.
    ///
    std::map<std::string, Map::Blocker> blocked_tiles;

    ///
    /// Map_objects which move with the sprite
    ///
    std::vector<std::shared_ptr<MapObject>> inventory;

public:
    Sprite();
    Sprite(int _x_position, int _y_position, std::string _name, int _sprite_sheet_id, std::string _sprite_sheet="../resources/characters_1.png");
    virtual ~Sprite();

    ///
    /// Set the sprite sheet to use for this character
    /// @param _sprite_sheet the sprite sheet
    ///
    void set_sprite_sheet(std::string _sprite_sheet);

    ///
    /// Get the sprite sheet
    /// @return the sprite sheet
    ///
    std::string get_sprite_sheet() { return sprite_sheet; }

    ///
    /// Set the id of the sprite in the sprite sheet
    /// @param _sprite_sheet_id the id of the sprite in the sprite sheet
    ///
    void set_sprite_sheet_id(int _sprite_sheet_id);

    ///
    /// Get the id of the sprite in the sprite sheet
    /// @return the sprite sheet id
    ///
    int get_sprite_sheet_id() { return sprite_sheet_id; }

    ///
    /// manage collisions for spirtes as they move
    /// @param target
    ///     tile the sprite it moving to
    ///
    void set_state_on_moving_start(Vec2D target);

    ///
    /// manage collisions for sprites as they move
    ///
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

    ///
    /// Get the object's text to display
    /// @return the object's text
    ///
    Text* get_object_text() {return object_text; }

    ///
    /// Set the object's text to be displayed 
    /// @param _object_text the object's text
    ///
    void set_object_text(Text* _object_text) {object_text = _object_text; }

    ///
    /// Get the object's status text
    /// @return the object's status text
    ///
    Text* get_status_text() {return status_text; }
    
    ///
    /// Set the object's status text
    /// @param _status_text the object's status text
    ///
    void set_status_text(Text* _status_text) {status_text = _status_text; }

    ///
    /// add map_object to sprites inventory
    ///
    void add_to_inventory(std::shared_ptr<MapObject> new_object);

    std::vector<std::shared_ptr<MapObject>> get_inventory() {return inventory; }

    void remove_from_inventory(std::shared_ptr<MapObject> old_object);

    bool is_in_inventory(std::shared_ptr<MapObject> object);

    /// TODO: add a remove from inventory method
    void set_y_position(int y_pos);
    void set_x_position(int x_pos);
    void set_y_position(double y_pos);
    void set_x_position(double x_pos);
};

#endif

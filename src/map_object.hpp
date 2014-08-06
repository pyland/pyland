#ifndef MAP_OBJECT_H
#define MAP_OBJECT_H

#include "image.hpp"
#include "object.hpp"
#include "text.hpp"
#include "text_font.hpp"
#include "typeface.hpp"
#include <string>

///
/// Represents an object which can be rendered on the map
///
class MapObject : public Object{

protected:
    ///
    /// The text to display above the object
    ///
    Text* object_text = nullptr;

    ///
    /// The status text for the object
    ///
    Text* status_text = nullptr;

    ///
    /// Walkable: determine if the object can be walked over
    ///
    Walkability walkability = Walkability::WALKABLE;


    ///
    /// The x position of the object
    ///
    double x_position = 0;

    ///
    /// The y position of the object
    ///
    double y_position = 0;

    ///
    /// Tiles that the object is blocking, probably
    /// by standing on.
    ///
    std::map<std::string, Map::Blocker> blocked_tiles;

    ///
    /// The object's moving state
    ///
    bool moving = false;


public:
    MapObject();
    MapObject(int _x_position, int _y_position, std::string _name);
    virtual ~MapObject();

    ///
    /// Set the object's x position in tiles
    /// @param x_pos the new x position in tiles
    ///
    void set_x_position(int x_pos) { x_position = x_pos; }
    void set_x_position(double x_pos) { x_position = x_pos; }

    ///
    /// Get the object's x position in tiles
    /// @return the object's x position in tiles
    ///
    double get_x_position() { return x_position; }

    ///
    /// Set the object's y position in tiles
    /// @param y_pos
    ///
    void set_y_position(int y_pos) { y_position = y_pos; }
    void set_y_position(double y_pos) { y_position = y_pos; }

    ///
    /// Get the object's y position in tiles
    /// @return the object's y position in tiles
    ///
    double get_y_position() { return y_position; }


   

    ///
    /// Get the object's walkability
    ///
    Walkability get_walkability() { return walkability; }

    ///
    /// Set the object's walkability
    /// @param _walkability the walkability of the object
    ///
    void set_walkability(Walkability _walkability) { walkability = _walkability; }

    // TODO Joshua: Comment
    void set_state_on_moving_start(Vec2D target);
    void set_state_on_moving_finish();

    ///
    /// Generate the texture coordinate data for the object
    ///
    void generate_tex_data();

    ///
    /// Generate the vertex data for the object
    ///
    void generate_vertex_data();

    ///
    /// Load the textures that are being used by the object
    ///
    void load_textures();

    ///
    /// Initialise the shaders that are being used by the object
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
    /// Set the object's moving status
    /// @param _moving if the object is moving
    ///
    void set_moving(bool _moving) { moving = _moving; }

    ///
    /// Get if the object is moving
    /// @return the object's moving status
    ///
    bool is_moving() { return moving; }
};

#endif

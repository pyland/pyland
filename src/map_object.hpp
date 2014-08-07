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
    /// Walkable: determine if the object can be walked over
    ///
    Walkability walkability = Walkability::WALKABLE;

    ///
    /// The name of the tilesheet to use for the map object
    ///
    std::string tile_sheet;

    ///
    /// The id of the tile in the sheet
    ///
    int tile_sheet_id;

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
    MapObject(int _x_position, int _y_position, std::string _name, int _tile_sheet_id, std::string _tile_sheet="../resources/basictiles_2.png");
    virtual ~MapObject();

    ///
    /// Set the tile sheet to use for this character
    /// @param _tile_sheet the tile sheet
    ///
    void set_tile_sheet(std::string _tile_sheet);

    ///
    /// Get the tile sheet
    /// @return the tile sheet
    ///
    std::string get_tile_sheet() { return tile_sheet; }

    ///
    /// Set the id of the tile in the tile sheet
    /// @param _tile_sheet_id the id of the tile in the tile sheet
    ///
    void set_tile_sheet_id(int _tile_sheet_id);

    ///
    /// Get the id of the tile in the tile sheet
    /// @return the tile sheet id
    ///
    int get_tile_sheet_id() { return tile_sheet_id; }

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

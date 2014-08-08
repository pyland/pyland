#ifndef MAP_OBJECT_H
#define MAP_OBJECT_H

#include <glm/vec2.hpp>
#include <string>

#include "image.hpp"
#include "object.hpp"
#include "text.hpp"
#include "text_font.hpp"
#include "typeface.hpp"

///
/// Represents an object which can be rendered on the map
///
class MapObject : public Object {
protected:
    ///
    /// The name of the tilesheet to use for the map object
    ///
    std::string sheet_name;

    ///
    /// The id of the tile in the sheet
    ///
    int sheet_id;

    ///
    /// The position of the object
    ///
    glm::vec2 position;

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
    MapObject(glm::vec2 position,
              std::string name,
              int sheet_id,
              std::string sheet_name="../resources/basictiles_2.png");

    virtual ~MapObject();

    glm::vec2 get_position() { return position; }
    void set_position(glm::vec2 position) { this->position = position; }

    ///
    /// Set the tile sheet to use for this character
    /// @param _tile_sheet the tile sheet
    ///
    void set_sheet_name(std::string sheed_name);

    ///
    /// Get the tile sheet
    /// @return the tile sheet
    ///
    std::string get_sheet_name() { return sheet_name; }

    ///
    /// Set the id of the tile in the tile sheet
    /// @param _tile_sheet_id the id of the tile in the tile sheet
    ///
    void set_sheet_id(int sheet_id);

    ///
    /// Get the id of the tile in the tile sheet
    /// @return the tile sheet id
    ///
    int get_sheet_id() { return sheet_id; }

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
    /// Set the object's moving status
    /// @param _moving if the object is moving
    ///
    void set_moving(bool moving) { this->moving = moving; }

    ///
    /// Get if the object is moving
    /// @return the object's moving status
    ///
    bool is_moving() { return moving; }
};

#endif

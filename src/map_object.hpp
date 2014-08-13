#ifndef MAP_OBJECT_H
#define MAP_OBJECT_H

#include <glm/vec2.hpp>
#include <string>
#include <vector>

#include "image.hpp"
#include "map.hpp"
#include "object.hpp"
#include "text.hpp"
#include "text_font.hpp"
#include "typeface.hpp"
#include "walkability.hpp"

///
/// Represents an object which can be rendered on the map
///
class MapObject : public Object {
protected:
    ///
    /// Render the object above sprites as an overlay
    ///
    bool render_above_sprite;

    ///
    /// Whether you can step all over it
    ///
    Walkability walkability;

    ///
    /// Re-create the blockers for blocking one's path
    ///
    void regenerate_blockers();

    ///
    /// The blockers for blocking one's path
    ///
    std::vector<Map::Blocker> body_blockers;

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
              Walkability walkability,
              int sheet_id,
              std::string sheet_name="../resources/basictiles_2.png");

    virtual ~MapObject();

    glm::vec2 get_position() { return position; }
    void set_position(glm::vec2 position) { this->position = position; }

    ///
    /// Determine if the object should be rendered above sprites
    /// @return if the object should render above sprites
    ///
    bool render_above_sprites() { return render_above_sprite; }

    ///
    /// If the object is to be rendered above sprites
    /// @param _render_above_sprites true if the object should be above sprites
    ///
    void set_render_above_sprites(bool _render_above_sprites) { render_above_sprite = _render_above_sprites; }

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
    /// Set whether the object creates blockers
    /// to prevent sprites moving onto the squares
    /// occupied by the sprite.
    ///
    /// @param walkability
    ///     The value to set the internal walkability to.
    ///
    void set_walkability(Walkability walkability);

    ///
    /// Get if the object is moving
    /// @return the object's moving status
    ///
    bool is_moving() { return moving; }
};

#endif

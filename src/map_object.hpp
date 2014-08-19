#ifndef MAP_OBJECT_H
#define MAP_OBJECT_H

#include <glm/vec2.hpp>
#include <map>
#include <string>
#include <vector>

#include "map.hpp"
#include "object.hpp"
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
    ///
    /// Constructs a map object
    /// @param position the (x, y) position of the map object on the map
    /// @param name the name of the map object
    /// @param walkability the walkability properties of the map object
    /// @param tile_name the name of the tile to render
    ///
    MapObject(glm::vec2 position,
              std::string name,
              Walkability walkability,
              std::pair<int, std::string> tile);

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
    /// Generate the texture coordinate data for the object
    ///
    void generate_tex_data(std::pair<int, std::string> tile);

    ///
    /// Change the tile of the sprite to that of the given name
    ///
    void set_tile(std::pair<int, std::string> tile);

    ///
    /// Generate the vertex data for the object
    ///
    void generate_vertex_data();

    ///
    /// Load the textures that are being used by the object
    ///
    void load_textures(std::pair<int, std::string> tile);

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

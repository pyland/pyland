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
    MapObject();
    MapObject(glm::vec2 position, std::string name);
    virtual ~MapObject();

    glm::vec2 get_position() { return position; }
    void set_position(glm::vec2 position) { this->position = position; }

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
    // TODO Joshua: Consider integer-ness.
    void set_state_on_moving_start(glm::ivec2 target);
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

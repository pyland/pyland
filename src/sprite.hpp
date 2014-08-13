#ifndef SPRITE_H
#define SPRITE_H

#include <algorithm>
#include <vector>

#ifdef USE_GLES
#include <GLES2/gl2.h>
#endif

#ifdef USE_GL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif

#include "image.hpp"
#include "map.hpp"
#include "map_object.hpp"
#include "typeface.hpp"
#include "text_font.hpp"
#include "text.hpp"
#include "engine.hpp"
#include "walkability.hpp"


enum class Sprite_Status {NOTHING, RUNNING, STOPPED, FAILED, KILLED};

///
/// Represents a sprite in the engine
///
class Sprite : public MapObject {
private:
    Sprite_Status string_to_status (std::string status);

protected:
    ///
    /// The text to display above the object
    ///
    Text *object_text = nullptr;

    ///
    /// The status text for the object
    ///
    Text *status_text = nullptr;

    ///
    /// status of sprite
    /// TODO: this value isn't used at the moment, use for status images
    Sprite_Status sprite_status;

    ///
    /// Tiles that the object is blocking, probably
    /// by standing on.
    ///
    std::map<std::string, Map::Blocker> blocked_tiles;

    ///
    /// Map_objects which move with the sprite
    ///
    std::vector<std::shared_ptr<MapObject>> inventory;

    ///
    /// The focus icon, to move with sprite and hide, depending on if sprite is in focus;
    ///
    bool is_focus;

    ///
    /// Overlays
    ///
    std::vector<int> overlay_ids;
    
    ///
    /// The dimensions of the overlays (width, height).
    /// 1.0 means the overlays is the same size as the engine's tile size * global scale.
    ///
    std::vector<std::pair<float, float>> overlay_dimensions;

    ///
    /// The offsets of the overlays (x, y ).
    /// (0.0, 0.0) is the bottom left of the sprite. These offsets follow
    /// cartesian quadrants in offsetting.
    ///  1.0 means the offset is the same size as the engine's tile size * global scale.
    ///
    std::vector<std::pair<float, float>> overlay_offsets;

    ///
    /// Underlays
    ///
    std::vector<int> underlay_ids;
    
    ///
    /// The dimensions of the underlays (width, height)
    /// 1.0 means the underlays is the same size as the engine's tile size * global scale.
    ///
    std::vector<std::pair<float, float>> underlay_dimensions;

    ///
    /// The offsets of the underlays (x, y ).
    /// (0.0, 0.0) is the bottom left of the sprite. These offsets follow
    /// cartesian quadrants in offsetting.
    ///  1.0 means the offset is the same size as the engine's tile size * global scale.
    ///
    std::vector<std::pair<float, float>> underlay_offsets;

public:
    Sprite();

    Sprite(glm::ivec2 position,
           std::string name,
           Walkability walkability,
           int sheet_id,
           std::string sheet_name="../resources/characters_1.png");

    virtual ~Sprite();

    ///
    /// Add an overlay to the sprite
    /// @param overlay_id The tile id
    ///
    void add_overlay(int overlay_id, float width=1.0f, float height=1.0f, float x_offset=0.0f, float y_offset=0.0f);

    ///
    /// Remove an overlay from the sprite
    /// @param overlay_id the tile id
    ///
    void remove_overlay(int overlay_id);

    ///
    /// Add an underlay to the sprite
    /// @param underlay_id
    /// 
    void add_underlay(int underlay_id, float width=1.0f, float height=1.0f, float x_offset=0.0f, float y_offset=0.0f);

    ///
    /// Remove an underlay from the sprite
    /// @param underlay_id the underlay id
    ///
    void remove_underlay(int underlay_id);

    ///
    /// manage collisions for spirtes as they move
    /// @param target
    ///     tile the sprite it moving to
    ///
    void set_state_on_moving_start(glm::ivec2 target);

    ///
    /// manage collisions for sprites as they move
    ///
    void set_state_on_moving_finish();

    ///
    /// Get the object's text to display
    /// @return the object's text
    ///
    Text* get_object_text() { return object_text; }

    ///
    /// Set the object's text to be displayed
    /// @param _object_text the object's text
    ///
    void set_object_text(Text* _object_text) {object_text = _object_text; }

    ///
    /// Get the object's status text
    /// @return the object's status text
    ///
    Text* get_status_text() { return status_text; }

    ///
    /// Set the object's status text
    /// @param _status_text the object's status text
    ///
    void set_status_text(Text* _status_text) {status_text = _status_text; }

    ///
    /// add map_object to sprites inventory
    ///
    void add_to_inventory(std::shared_ptr<MapObject> new_object);

    std::vector<std::shared_ptr<MapObject>> get_inventory() { return inventory; }

    void set_position(glm::vec2 position);


    ///
    /// Generate the texture coordinate data for the object
    ///
    void generate_tex_data();

    ///
    /// Generate the vertex data for the object
    ///
    void generate_vertex_data();


    ///
    /// remove the specified object from the sprites inventory, safe to use even if
    /// item isn't in inventory
    /// @return
    ///     true if successfully removed, false if it wasn't present
    bool remove_from_inventory(std::shared_ptr<MapObject> old_object);

    bool is_in_inventory(std::shared_ptr<MapObject> object);

    void set_y_position(int y_pos);
    void set_x_position(int x_pos);
    void set_y_position(double y_pos);
    void set_x_position(double x_pos);

    void set_sprite_status(std::string _sprite_status);

    void set_focus(bool _is_focus);
};

#endif

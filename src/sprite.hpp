#ifndef SPRITE_H
#define SPRITE_H

#include <glm/vec2.hpp>
#include <map>
#include <string>
#include <vector>

#include "map.hpp"
#include "map_object.hpp"
#include "walkability.hpp"

class Text;

enum class Sprite_Status {NOTHING, RUNNING, STOPPED, FAILED, KILLED};

///
/// Represents a sprite in the engine
///
class Sprite : public MapObject {
private:
    Sprite_Status string_to_status (std::string status);

    std::string sprite_name;

protected:
    ///
    /// The text to display above the object
    ///
    Text *object_text = nullptr;

    ///
    /// The status text for the object
    ///
    int status_icon_id;

    ///
    /// status of sprite
    /// TODO: this value isn't used at the moment, use for status images
    Sprite_Status sprite_status;


    ///
    /// Map_objects which move with the sprite
    ///
    std::vector<int> inventory;

    ///
    /// The focus icon, to move with sprite and hide, depending on if sprite is in focus;
    ///
    bool is_focus;

    ///
    /// The focus icon, to move with sprite and hide, depending on if sprite is in focus;
    ///
    int focus_icon_id;

    ///
    /// Tiles that the object is blocking, probably
    /// by standing on.
    ///
    std::map<std::string, Map::Blocker> blocked_tiles;

public:
    Sprite();
    ///
    /// Constructs a sprite
    /// @param position
    ///     the (x, y) position of the sprite on the map
    /// @param name
    ///     the name of the sprite
    /// @param walkability
    ///     the walkability properties of the sprite
    /// @param tile
    ///     the sprite's imaage, referenced by an id:sheet_name pair
    ///
    Sprite(glm::ivec2 position,
           std::string name,
           Walkability walkability,
           std::pair<int, std::string> tile);

    virtual ~Sprite();

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
    /// add map_object to sprites inventory
    ///
    void add_to_inventory(int new_object);

    std::vector<int> get_inventory() { return inventory; }

    void set_position(glm::vec2 position);


    ///
    /// remove the specified object from the sprites inventory, safe to use even if
    /// item isn't in inventory
    /// @return
    ///     true if successfully removed, false if it wasn't present
    bool remove_from_inventory(int old_object);

    bool is_in_inventory(int object);


    void set_sprite_status(std::string _sprite_status);

    void set_focus(bool _is_focus);

    std::string get_sprite_name() {return sprite_name; }
};


#endif

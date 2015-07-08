#ifndef MAP_OBJECT_H
#define MAP_OBJECT_H

#include <memory>
#include <string>

#include "renderable_component.hpp"
#include "walkability.hpp"
#include "challenge.hpp" //added, not sure if should be here? TODO: BLEH check
#include "map.hpp"
#include "object.hpp"


class LockableEntityThread;
class Text;

///
/// The class to hold an object's information so that the Engine can
/// manipulate it.
///
class MapObject : public Object {

private:
    ///
    /// Boolean to indicate if the object has set itself to be renderable
    ///
    bool renderable = true;

    ///
    /// The object's id
    ///
    int id = 0;

protected:

    ///
    /// The object's renderable component
    ///
    RenderableComponent renderable_component;

    ///
    /// The name of the object
    ///
    std::string name;

    ///
    /// The position of the object
    ///
    glm::vec2 position;

    ///
    /// Whether you can step all over it
    ///
    Walkability walkability;

    ///
    /// The challenge that created or now owns the object.
    ///
    /// This must be set manually by the challenge,
    /// and may be null at any time.
    ///
    Challenge *challenge = nullptr;

    ///
    /// Re-create the blockers for blocking one's path
    ///
    void regenerate_blockers();

    ///
    /// The blockers for blocking one's path
    ///
    std::vector<Map::Blocker> body_blockers;

    ///
    /// The object's moving state
    ///
    bool moving = false;

    ///
    /// The text to display above the object
    ///
    Text *object_text = nullptr;

public:
    //MapObject();
    //MapObject(std::string name); TODO BLEH remove

    ///
    /// Constructs an MapObject
    /// @param position
    ///     the (x, y) position of the sprite on the map
    /// @param name
    ///     the name of the sprite
    /// @param walkability
    ///     the walkability properties of the sprite
    /// @param tile
    ///     the sprite's image, referenced by an id:sheet_name pair
    /// @param walk_frames
    ///     walking frames to animate movement.
    ///
    MapObject(glm::vec2 position,
              std::string name,
              Walkability walkability,
              AnimationFrames walk_frames,
              std::string start_frame);

    // "= default" not supported in g++-4.8
    virtual ~MapObject();

    ///
    /// DO NOT USE THIS! ONLY THE ENGINE SHOULD USE THIS FUNCTION
    /// Set the id of the object
    /// @param new_id the object's id
    ///
    void set_id(int new_id);

    ///
    /// Get the id of the object
    /// @return the id of the object
    ///
    int get_id() { return id; }

    ///
    /// Set the object's name
    /// @param new_name the name of the object
    ///
    void set_name(std::string new_name);

    ///
    /// Get the name of the object
    /// @return the name of the object
    std::string get_name() { return name; }

    ///
    /// Get the renderable component
    /// @return the renderable component for this object
    ///
    RenderableComponent* get_renderable_component() { return &renderable_component; }

    ///
    /// Determine if the object can be rendered
    /// @return boolean value: true if the object can be rendered and false if not
    ///
    bool is_renderable() { return renderable; }

    ///
    /// Set whether the object can be rendered
    /// @param can_render true if the object can be rendered and false if not
    ///
    void set_renderable(bool can_render) { renderable = can_render; }

    ///
    /// The Python thread for running scripts in.
    ///
    std::unique_ptr<LockableEntityThread> daemon;
    

    ///
    /// Get the position of the object on the map
    ///
    glm::vec2 get_position() { return position; }
    
    ///
    /// Set the position of the object on the map
    ///
    virtual void set_position(glm::vec2 position);

    ///
    /// Get if the object is moving
    /// @return the object's moving status
    ///
    virtual bool is_moving() { return moving; }

    ///
    /// manage collisions for spirtes as they move
    /// @param target
    ///     tile the sprite it moving to
    ///
    virtual void set_state_on_moving_start(glm::ivec2 target);

    ///
    /// manage collisions for sprites as they move
    ///
    virtual void set_state_on_moving_finish();

    ///
    /// Change the tile of the sprite to that of the given name
    ///
    virtual void set_tile(std::pair<int, std::string> tile);

    ///
    /// Walking frames to animate movement.
    ///created
    AnimationFrames frames;

    ///
    /// Get the object's text to display
    /// @return the object's text
    ///
    Text* get_object_text() { return object_text; }

    void set_object_status(std::string _object_status); //TODO BLEH investigate why this is here

    void set_focus(bool _is_focus); //TODO BLEH find out what this does

};

#endif

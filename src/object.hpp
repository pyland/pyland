#ifndef OBJECT_H
#define OBJECT_H

#include <memory>
#include <string>

#include "renderable_component.hpp"


class LockableEntityThread;

///
/// The class to hold an object's information so that the Engine can
/// manipulate it.
///
class Object {

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

public:
    Object();
    Object(std::string name);

    // "= default" not supported in g++-4.8
    virtual ~Object();

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

    ///TODO: Joshua comment
    std::unique_ptr<LockableEntityThread> daemon;
};

#endif

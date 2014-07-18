#ifndef OBJECT_H
#define OBJECT_H

#include "renderable_component.hpp"
#include <string>

///
/// The class to hold an object's information so that the Engine can
/// manipulate it.
///
///
///
class Object {
  
    ///
    /// The object's renderable component
    ///
    RenderableComponent renderable_component;

    ///
    /// Boolean to indicate if the object has set itself to be renderable
    ///
    bool renderable = true;
    
    ///
    /// The x position of the object
    ///
    int x_position = 0;

    ///
    /// The y position of the object
    ///
    int y_position = 0;
  
    ///
    /// The object's id
    ///
    int id = -1;
  
    ///
    /// The name of the object
    ///
    std::string name = "";

    ///
    /// The python script to run for this object
    ///
    std::string script = "";

    /// 
    /// Set the id of the object
    /// @param new_id the object's id
    ///
    void set_id(int new_id);

public:
    Object();
    ~Object();
    ///
    /// Set the object's x position
    /// @param x_pos the new x position
    ///
    void set_x_position(int x_pos);

    ///
    /// Get the object's x position
    /// @return the object's x position
    ///
    int get_x_position() { return x_position; }

    ///
    /// Set the object's y position
    /// @param y_pos
    ///
    void set_y_position(int y_pos);

    ///
    /// Get the object's y position
    /// @return the object's y position
    ///
    int get_y_position() { return y_position; }


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
    /// Set the python script to use for this object
    /// @param new_script the script to use for the object
    ///
    void set_script(std::string new_script);
  
    ///
    /// Get the python script to be used for this object
    /// @return the script to use for the object
    ///
    std::string get_script() { return script; }

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
};

#endif

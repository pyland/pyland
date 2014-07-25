#ifndef COMPONENT_H
#define COMPONENT_H

#include <functional>
#include <memory>
#include <utility>
#include <vector>

#ifdef USE_GLES
#include <GLES2/gl2.h>
#endif

#ifdef USE_GL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif


///
/// A GUI component that is the base class which all renderable
/// components derive from. It is an abstract base class
///
class Component {
protected:
    
    ///
    /// The Opengl vertex data for this component, transformed into world space
    ///
    GLfloat* vertex_data;

    ///
    /// The Opengl texture data for this component
    ///
    GLfloat* texture_data;

    ///
    /// A pointer to the parent
    ///
    //    std::weak_pr<Component> parent;

    ///
    /// The GUID of the component
    ///
    int id;

    ///
    /// Width of the component relative to the parent
    ///
    float width;
    
    ///
    /// Height of the component relative to the parent
    ///
    float height;

    ///
    /// The width of the component in pixels - used to generate the
    /// vertex data
    ///
    int width_pixels;

    ///
    /// The height of the component in pixels - used to generate the
    /// vertex data
    ///
    int height_pixels;

    ///
    /// The x offset of the component relative to the parent
    ///
    float x_offset;

    ///
    /// The y offset of the component relative to the parent
    ///
    float y_offset;

    ///
    /// The lambda function that is called when the button is clicked
    ///
    std::function<void (void)> on_click_func;

public:
    Component();
    ~Component();
    ///
    /// Generates the vertex data for this particular component. This
    /// data is in the local 'object' space and will need to be
    /// transformed by a manager into the global vertex data.
    /// The pair holds the pointer and then the size of the data in bytes
    ///
    virtual std::vector<std::pair<GLfloat*, int>> generate_vertex_data() = 0;

    ///
    /// Same as the vertex function but generates texture data
    /// @param component the component to generate the data for 
    /// The pair holds the pointer and then the size of the data in bytes
    ///
    virtual std::vector<std::pair<GLfloat*, int>> generate_texture_data() = 0;

    ///
    /// Set the on click lambda function for this button
    /// @param func the lambda function
    /// @return if the function registered correctly
    ///
    bool set_on_click(std::function<void (void)> func);

    ///
    /// Clear the handler - replaces with a void lambda that does nothing
    ///
    void clear_on_click();

    ///
    /// Set the component's id
    /// @param new_id the id of the component
    ///
    void set_id(int new_id) { id = new_id; }

    ///
    /// Get the id of the component
    /// @return the id
    int get_id() { return id; }

    ///
    /// Set the  width of the component relative to its parent
    /// @param _width the width
    ///
    void set_width(float _width) { width = _width; }

    ///
    /// Get the width of the component relative to its parent
    /// @return the width
    ///
    float get_width() { return width; }

    ///
    /// Set the  height of the component relative to its parent
    /// @param _height the height
    ///
    void set_height(float _height) { height = _height; }

    ///
    /// Get the height of the component relative to its parent
    /// @return the height

    ///
    float get_height() { return height; }

    ///
    /// Get the y offest of the component relative  to its parent
    /// @return the y offset
    ///
    float get_y_offset() { return y_offset; }

    ///
    /// Set the y offset of the component relative to its parent
    /// @param _y_offset the y offset of the component
    ///
    void set_y_offset(float _y_offset) { y_offset = _y_offset; }

    ///
    /// Get the x offest of the component relative  to its parent
    /// @return the x offset
    ///
    float get_x_offset() { return x_offset; }

    ///
    /// Set the x offset of the component relative to its parent
    /// @param _x_offset the x offset of the component
    ///
    void set_x_offset(float _x_offset) { x_offset = _x_offset; }

};

#endif

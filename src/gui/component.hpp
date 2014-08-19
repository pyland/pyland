#ifndef COMPONENT_H
#define COMPONENT_H


#include <exception>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>

#ifdef USE_GLES
#include <GLES2/gl2.h>
#endif

#ifdef USE_GL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif

class GUIText;
class TextureAtlas;

///
/// A GUI component that is the base class which all renderable
/// components derive from. It is an abstract base class
///
class Component {
protected:
    ///
    /// The parent of this component
    ///
    Component* parent;

    ///
    /// Mutex to lock on for getting new ids
    ///
    std::mutex component_mutex;

    ///
    /// The Opengl vertex data for this component, transformed into world space
    ///
    GLfloat* vertex_data;

    ///
    /// The number of bytes in the vertex data
    ///
    int size_vertex_data;

    ///
    /// The Opengl texture data for this component
    ///
    GLfloat* texture_data;

    ///
    /// The size of the texture data in bytes
    ///
    int size_texture_data;

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
    /// The x offset of the component relative to the parent. This is the bottom left.
    ///
    float x_offset;

    ///
    /// The y offset of the component relative to the parent. This is the bottom left.
    ///
    float y_offset;

    ///
    /// The x offset of the component in pixels, relative to the parent
    ///
    int x_offset_pixels;

    ///
    /// The y offset of the component in pixels, relative to the parent
    ///
    int y_offset_pixels;

    ///
    /// Indicates if the component is clickable
    ///
    bool clickable;
    ///
    /// The lambda function that is called when the button is clicked
    ///
    std::function<void (void)> on_click_func;

    ///
    /// If the component is visible
    ///
    bool visible;

    ///
    /// Get the next unique identifier for the component - starting at 1.
    ///
    int get_new_id();


    ///
    /// The texture atlas being used for this GUI - 
    /// 
    std::shared_ptr<TextureAtlas> texture_atlas;
public:
    Component();
    Component(std::function<void (void)> on_click, float _width, float _height, float _xo_offset, float _y_offset);
 
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
    /// The pair holds the pointer and then the size of the data in bytes
    ///
    virtual std::vector<std::pair<GLfloat*, int>> generate_texture_data() = 0;

    ///
    /// Generates the text data for this component
    /// 
    /// 
    virtual std::vector<std::shared_ptr<GUIText>> generate_text_data() = 0;

    ///
    /// Get the map listing all the components of the group.
    /// This returns an empty map for Component.
    /// @return a reference to a constant map object
    ///
    virtual const std::map<int, std::shared_ptr<Component>>& get_components();

    ///
    /// Set the visibility
    /// @parma _visible
    ///
    void set_visible(bool _visible) { visible = _visible; }

    ///
    /// Get the visibility
    /// @return visibility
    ///
    bool is_visible() { return visible; }

    ///
    /// Set the on click lambda function for this button
    /// @param func the lambda function
    /// @return if the function registered correctly
    ///
    void set_on_click(std::function<void (void)> func);

    ///
    /// Call the component's on click function
    ///
    void call_on_click() { on_click_func(); }

    ///
    /// Clear the handler - replaces with a void lambda that does nothing
    ///
    void clear_on_click();

    ///
    /// Sets if this component can be clicked
    /// @param _clickable indicate if the component can be clicked
    ///
    void set_clickable(bool _clickable) { clickable = _clickable; }

    ///
    /// Determine if this component is clickable
    ///
    bool is_clickable() { return clickable; }

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
    /// Set this component's parent
    /// @_parent the parent
    ///
    void set_parent(Component* _parent) { parent = _parent; }

    ///
    /// Get the parent of this component
    /// @return the parent
    /// TODO:make these weak ptrs
    Component* get_parent() { return parent; }

    ///
    /// Set the  width of the component relative to its parent
    /// @param _width the width
    ///
    void set_width(float _width);

    ///
    /// Get the width of the component relative to its parent
    /// @return the width
    ///
    float get_width() { return width; }

    ///
    /// Set the width of the component in pixels
    /// @param pixels the width of the component
    ///
    void set_width_pixels(int pixels) { width_pixels = pixels; }

    ///
    /// Get the width of the component in pixels
    /// @return the width in pixels
    ///
    int get_width_pixels() { return width_pixels; }

    ///
    /// Set the height of the component in pixels
    /// @pixels the height
    ///
    void set_height_pixels(int pixels) { height_pixels = pixels; }

    ///
    /// Get the height of the component in pixels
    /// @param height_pixels the height of the component
    ///
    int get_height_pixels() { return height_pixels; }

    ///
    /// Set the  height of the component relative to its parent
    /// @param _height the height
    ///
    void set_height(float _height);

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
    /// Set the x offset of the component in pixels, relative to its parent
    /// @param pixels set the offset of this component in pixels
    ///
    void set_x_offset_pixels(int pixels) { x_offset_pixels = pixels; }

    ///
    /// Get the x offset of the component in pixels, relative to its parent
    /// @param pixels the offset
    ///
    int get_x_offset_pixels() { return x_offset_pixels; }

    ///
    /// Set the y offset of the component in pixels, relative to its parent
    /// @param pixels set the offset of this component in pixels
    ///
    void set_y_offset_pixels(int pixels) { y_offset_pixels = pixels; }

    ///
    /// Get the y offset of the component in pixels, relative to its parent
    /// @param pixels the offset
    ///
    int get_y_offset_pixels() { return y_offset_pixels; }


    ///
    /// Set the x offset of the component relative to its parent
    /// @param _x_offset the x offset of the component
    ///
    void set_x_offset(float _x_offset) { x_offset = _x_offset; }

    ///
    /// Set the texture atlas being used
    /// @param _texture_atlas  the texture atlas
    ///
    void set_texture_atlas(std::shared_ptr<TextureAtlas> _texture_atlas);
};

class component_no_children_exception : public std::exception {
    virtual const char* what() const throw() {
        return "Component has no children";
    }
};

#endif

#ifndef COMPONENTGROUP_H
#define COMPONENTGROUP_H

#include <map>
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

#include "component.hpp"


///
/// A component group holds components and is itself a sub class of
/// component. This means that component groups can hold component
/// groups, allowing us to build up a component tree of components to render.
///
class ComponentGroup : public Component {
protected:
    ///
    /// The number of components in the group
    ///
    std::map<int, std::shared_ptr<Component>> components;
public:

    ComponentGroup();
    ComponentGroup(std::function<void (void)> on_click, float _width, float _height, float _xo_offset, float _y_offset);


    ~ComponentGroup();

    ///
    /// Add a component to the component group
    /// @param component the component to add the the group
    ///
    void add(std::shared_ptr<Component> component);

    ///
    /// Remove a component from the group
    /// @param component_id the id of the component
    ///
    void remove(int component_id);

    ///
    /// Get a component shared_ptr by id
    /// @param component_id the id of the component
    /// @return a shared_ptr to the component
    ///
    std::shared_ptr<Component> get_component(int component_id);

    ///
    /// Get the map listing all the components of the group
    /// @return a reference to a constant map object
    ///
    const std::map<int, std::shared_ptr<Component>> * get_components();

    ///
    /// Generates the vertex data for this particular component. This
    /// data is in the local 'object' space and will need to be
    /// transformed by a manager into the global vertex data.
    /// The pair holds the pointer and then the size of the data in bytes
    ///
    std::vector<std::pair<GLfloat*, int>> generate_vertex_data();

    ///
    /// Same as the vertex function but generates texture data
    /// The pair holds the pointer and then the size of the data in bytes
    ///
    std::vector<std::pair<GLfloat*, int>> generate_texture_data();

    ///
    /// Gets a vector of all the GUIText elements for this component
    ///
    std::vector<std::shared_ptr<GUIText>> generate_text_data();

    ///
    /// This function is called by generate vertex data (See Call Super on wikipedia)
    /// to generate the vertex data for this actual component. We need to also
    /// enumerate all the components in this group and so this behaviour is common to
    /// all subclasses.
    /// The pair holds the pointer and then the size of the data in bytes
    ///
    virtual std::vector<std::pair<GLfloat*, int>> generate_this_vertex_data() = 0;

    ///
    /// This function is called by generate texture data (See Call Super on wikipedia)
    /// to generate the texture data for this actual component
    /// The pair holds the pointer and then the size of the data in bytes
    ///
    virtual std::vector<std::pair<GLfloat*, int>> generate_this_texture_data() = 0;

    ///
    /// This function is called by generate text data (See Call Super on wikipedia)
    /// to generate the text data for this actual component
    ///
    virtual std::vector<std::shared_ptr<GUIText>> generate_this_text_data() = 0;

};
#endif

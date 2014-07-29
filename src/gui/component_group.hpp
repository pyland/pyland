#ifndef COMPONENTGROUP_H
#define COMPONENTGROUP_H

#include "component.hpp"
#include "python_embed_headers.hpp"

#include <map>
#include <memory>

///
/// A component group holds components and is itself a sub class of
/// component. This means that component groups can hold component
/// groups, allowing us to build up a component tree of components to render.
///
class ComponentGroup : public Component {

    ///
    /// The number of components in the group
    ///
    std::map<int, std::shared_ptr<Component>> components;
public:

    ComponentGroup();
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
    const std::map<int, std::shared_ptr<Component>>& get_components();

    ///
    /// Generates the vertex data for this particular component. This
    /// data is in the local 'object' space and will need to be
    /// transformed by a manager into the global vertex data.
    /// The pair holds the pointer and then the size of the data in bytes
    ///
    std::vector<std::pair<GLfloat*, int>> generate_vertex_data();

    ///
    /// Same as the vertex function but generates texture data
    /// @param component the component to generate the data for 
    /// The pair holds the pointer and then the size of the data in bytes
    ///
    std::vector<std::pair<GLfloat*, int>> generate_texture_data();

    ///
    /// This function is called by generate vertex data (See Call Super on wikipedia)
    /// to generate the vertex data for this actual component. We need to also
    /// enumerate all the components in this group and so this behaviour is common to
    /// all subclasses.
    /// @param component the component to generate the data for 
    /// The pair holds the pointer and then the size of the data in bytes
    ///
    virtual std::vector<std::pair<GLfloat*, int>> generate_this_vertex_data() = 0;

    ///
    /// This function is called by generate texture data (See Call Super on wikipedia)
    /// to generate the texture data for this actual component
    /// @param component the component to generate the data for 
    /// The pair holds the pointer and then the size of the data in bytes
    ///
    virtual std::vector<std::pair<GLfloat*, int>> generate_this_texture_data() = 0;
};
#endif

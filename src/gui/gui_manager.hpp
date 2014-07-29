#ifndef GUI_MANAGER_H
#define GUI_MANAGER_H


#include "component.hpp"
#include "input_manager.hpp"
#include "callback.hpp"
#include "lifeline.hpp"
#include "filters.hpp"
#include "mouse_input_event.hpp"
#include "object.hpp"

#include <memory>

///
/// The GUIManager class manages all the components which have been
/// added to the component tree. It is responsible for parsing the
/// tree and getting its vertex and texture data to render the GUI.
///
/// The class inherits from object so that it can be added to the
/// rendering system and we batch all the geometry together to speed
/// up the rendering calls. It is responsible for dispatching events
/// to the required component e.g. mouse clicks.
///
class GUIManager : public Object {

    ///
    /// The texture data for this component
    ///
    GLfloat* gui_tex_data;

    ///
    /// The vertex data for this component
    ///
    GLfloat* gui_data;

    ///
    /// The texture (loaded image) for this component
    ///
    char* tex_buf;


    ///
    /// The root component of the tree
    ///
    std::shared_ptr<Component> root;


    ///
    /// Generate the texture data for this component and its sub components
    ///
    void generate_tex_data();

    ///
    /// Generate the vertex data for this component and its sub components
    ///
    void generate_vertex_data();

    ///
    /// Load the texture for this component
    ///
    void load_textures();

    ///
    /// Initialise the shaders used by this component
    /// @return boolean indicating success or failure of the operation (true is success)
    bool init_shaders();
public:
    GUIManager();
    ~GUIManager();

    ///
    /// Callback function to handle mouse events
    /// @param event the mouse event to be handled
    ///
    void mouse_callback_function(MouseInputEvent event);

    ///
    /// Update components in the component tree
    ///
    void update_components();

    ///
    /// Parse all the components and generate the vertex and texture data
    ///
    void  parse_components();

    ///
    /// Set the root component of the component tree
    /// @param component the root
    ///
    void set_root(std::shared_ptr<Component> component) { root = component; }

    ///
    /// Get the root component of the tree
    /// @return the root
    ///
    std::shared_ptr<Component> get_root() { return root; }


};

#endif

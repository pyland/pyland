#ifndef GUI_MANAGER_H
#define GUI_MANAGER_H

#include "component.hpp"
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
    GLfloat* gui_tex_data;
    GLfloat* gui_data;
    GLfloat* tex_buf;


///
/// The root component of the tree
///
std::shared_ptr<Component> root;

///
/// Parse all the components and generate the vertex and texture data
///
void  parse_components();

void generate_tex_data();
void generate_vertex_data();
void load_textures();
bool init_shaders();
public:
GUIManager();
~GUIManager();
void update_components();

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

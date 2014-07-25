#include "gui_window.hpp"

#ifdef USE_GLES
#include <GLES2/gl2.h>
#endif

#ifdef USE_GL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif

std::vector<std::pair<GLfloat*, int>> GUIWindow::generate_vertex_data() {
    
}

std::vector<std::pair<GLfloat*, int>> GUIWindow::generate_texture_data() {


}


#ifndef GUIWINDOW_H
#define GUIWINDOW_H

#include <utility>
#include <vector>

#ifdef USE_GL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif

#ifdef USE_GLES
#include <GLES2/gl2.h>
#endif

#include "component_group.hpp"

///
/// Window class to display a window
///
class GUIWindow : public ComponentGroup {

public:
    std::vector<std::pair<GLfloat*, int>> generate_this_vertex_data() override;

    std::vector<std::pair<GLfloat*, int>> generate_this_texture_data() override;

    std::vector<std::shared_ptr<GUIText>> generate_this_text_data() override;
};

#endif

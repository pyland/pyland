#ifndef GUIWINDOW_H
#define GUIWINDOW_H

#include "component_group.hpp"

///
/// Window class to display a window
///
class GUIWindow : public ComponentGroup {
    
public:
    std::vector<std::pair<GLfloat*, int>> generate_this_vertex_data() override;

    std::vector<std::pair<GLfloat*, int>> generate_this_texture_data() override;

};

#endif

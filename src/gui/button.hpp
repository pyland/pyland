#ifndef BUTTON_H
#define BUTTON_H

#include "component_group.hpp"
#include "gui_text.hpp"
#include "gui_text_data.hpp"
#include "text.hpp"



#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>


#ifdef USE_GLES
#include <GLES2/gl2.h>
#endif

#ifdef USE_GL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif


class Button : public ComponentGroup {
    std::shared_ptr<GUIText> button_text;
public:
    Button();
    Button(std::shared_ptr<Text> _text, std::function<void (void)> on_click, float _width, float _height, float _x_offset, float _y_offset);

    std::shared_ptr<Text> get_text();

    void set_text(std::shared_ptr<Text>  _text);

    std::vector<std::pair<GLfloat*, int>> generate_this_vertex_data();

    std::vector<std::pair<GLfloat*, int>> generate_this_texture_data();

    std::vector<std::shared_ptr<GUITextData>> generate_this_text_data();

};

#endif

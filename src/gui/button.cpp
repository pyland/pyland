#include "button.hpp"
#include "gui_text.hpp"
#include "text.hpp"
#include <utility>
#include <vector>

#ifdef USE_GLES
#include <GLES2/gl2.h>
#endif

#ifdef USE_GL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif


Button::Button() {
    button_text = std::make_shared<GUIText>();
    add(button_text);
}

Button::Button(std::shared_ptr<Text>  _text, std::function<void (void)> on_click, 
               float _width, float _height, float _x_offset, float _y_offset) :
    ComponentGroup(on_click, _width, _height, _x_offset, _y_offset) {
    button_text = std::make_shared<GUIText>();
    button_text->set_text(_text);
    button_text->set_width(0.7f);
    button_text->set_height(0.8f);
    button_text->set_x_offset(0.1f);
    button_text->set_y_offset(0.1f);
    add(button_text);
}

std::shared_ptr<Text> Button::get_text() {
    return button_text->get_text();
}

void Button::set_text(std::shared_ptr<Text> _text) {
    button_text->set_text(_text);
}

std::vector<std::pair<GLfloat*, int>> Button::generate_this_vertex_data() {
    delete []vertex_data;
    int num_floats = 12;

    vertex_data = new GLfloat[sizeof(GLfloat) * num_floats];
    //Generate coordinates in our local object space
    //bottom left
    vertex_data[0] = 0;
    vertex_data[1] = 0;

    //top left
    vertex_data[2] = 0;
    vertex_data[3] = (GLfloat)height_pixels;

    //bottom right
    vertex_data[4] = (GLfloat)width_pixels;
    vertex_data[5] = 0;

    //top left
    vertex_data[6] = 0;
    vertex_data[7] = (GLfloat)height_pixels;

    //top right
    vertex_data[8] = (GLfloat)width_pixels;
    vertex_data[9] = (GLfloat)height_pixels;

    //bottom right
    vertex_data[10] = (GLfloat)width_pixels;
    vertex_data[11] = 0;

    size_vertex_data = num_floats;

    std::vector<std::pair<GLfloat*, int>> vertices;
    vertices.push_back(std::make_pair(vertex_data, num_floats));
    return vertices;
}

std::vector<std::pair<GLfloat*, int>> Button::generate_this_texture_data() {
    delete []texture_data;
    int num_floats = 12;
    //TODO: fix this hack
    GLfloat offset_x = (GLfloat)0.0f;//GLfloat(1.0f / 16.0f);
    GLfloat offset_y = (GLfloat)0.0f;//GLfloat(1.0f /16.0f);
    texture_data = new GLfloat[sizeof(GLfloat)*num_floats];
    //bottom left
    texture_data[0]  = offset_x * GLfloat(4.0);
    texture_data[1]  = offset_y;

    //top left
    texture_data[2]  = offset_x * GLfloat(4.0);
    texture_data[3]  = 0.0f;

    //bottom right
    texture_data[4]  = offset_x * GLfloat(5.0);
    texture_data[5]  = offset_y;

    //top left
    texture_data[6]  = offset_x * GLfloat(4.0);
    texture_data[7]  = 0.0f;

    //top right
    texture_data[8]  = offset_x * GLfloat(5.0);
    texture_data[9]  = 0.0f;

    //bottom right

    texture_data[10] = offset_x * GLfloat(5.0);
    texture_data[11] = offset_y;
    size_texture_data = num_floats;
    std::vector<std::pair<GLfloat*, int>> texture_coords;
    texture_coords.push_back(std::make_pair(texture_data, num_floats));
    return texture_coords;
}

std::vector<std::shared_ptr<GUIText>> Button::generate_this_text_data() {
    std::vector<std::shared_ptr<GUIText>> text_data;
    text_data.push_back(button_text);
    return text_data;
}

#ifndef BUTTON_H
#define BUTTON_H

#include "component.hpp"

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


class Button : public Component {
std::string text;
public:
    
///
/// Set the text for the button
/// @param _text the button text
///
void set_text(std::string _text) { text = _text; }
            
///
///o Get the button's text
/// @return the text
///
std::string get_text() { return text; }

std::vector<std::pair<GLfloat*, int>> generate_vertex_data();

std::vector<std::pair<GLfloat*, int>> generate_texture_data();

};

#endif

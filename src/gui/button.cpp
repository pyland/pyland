#include "button.hpp"

#include <utility>
#include <vector>

#ifdef USE_GLES
#include <GLES2/gl2.h>
#endif

#ifdef USE_GL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif

std::vector<std::pair<GLfloat*, int>> Button::generate_vertex_data() {
    delete []vertex_data;
    int num_floats = 18;
    int scale = 32;

    //Generate coordinates in our local object space
    //bottom left 
    vertex_data[0] = 0;
    vertex_data[1] = 0;
    vertex_data[2] = 0;

    //top left
    vertex_data[3] = 0;
    vertex_data[4] = (1) * scale;
    vertex_data[5] = 0;

    //bottom right
    vertex_data[6] = (1) * scale;
    vertex_data[7] = 0;
    vertex_data[8] = 0;

    //top left
    vertex_data[9] = 0;
    vertex_data[10] = 1 * scale;
    vertex_data[11] = 0;

    //top right
    vertex_data[12] = 1 * scale;
    vertex_data[13] = 1 * scale;
    vertex_data[14] = 0;

    //bottom right
    vertex_data[15] = 1 * scale;
    vertex_data[16] = 0;
    vertex_data[17] = 0;

    

    vertex_data = new GLfloat[sizeof(GLfloat)*num_floats];
}

std::vector<std::pair<GLfloat*, int>> Button::generate_texture_data() {
    delete []texture_data;
    int num_floats = 12;

    GLfloat offset_x = GLfloat(16.0f);
    GLfloat offset_y = GLfloat(16.0f);

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

    texture_data = new GLfloat[sizeof(GLfloat)*num_floats];
}


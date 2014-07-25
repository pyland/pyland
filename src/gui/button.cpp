#include "button.hpp"

#include <memory>
#include <utility>
#include <vector>

#ifdef USE_GLES
#include <GLES2/gl2.h>
#endif

#ifdef USE_GL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif

std::vector<std::pair<std::shared_ptr<GLfloat>, int>> generate_vertex_data() {
    
}

std::vector<std::pair<std::shared_ptr<GLfloat>, int>> generate_texture_data() {


}


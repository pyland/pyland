#ifndef MOUSE_CURSOR_H
#define MOUSE_CURSOR_H

#include <memory>


extern "C" {
#ifdef USE_GL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif

#ifdef USE_GLES
#include <GLES2/gl2.h>
#endif
}


#include "lifeline.hpp"
#include "renderable_component.hpp"



class GameWindow;
class Shader;
class TextureAtlas;



///
/// A simple mouse cursor which follows the mouse.
///
/// This is used on systems where the EGL surface is in-front of the
/// usual desktop mouse.
///
class MouseCursor {
private:
    GameWindow* window;
    Lifeline callback_lifeline;
    std::shared_ptr<TextureAtlas> atlas;
    std::shared_ptr<Shader> shader;
    GLuint vbo;
    int x;
    int y;
    float tex_x1;
    float tex_y1;
    float tex_x2;
    float tex_y2;
    bool dirty;
public:
    MouseCursor(GameWindow* window);
    ~MouseCursor();
    void display();
};



#endif

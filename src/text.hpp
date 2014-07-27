#ifndef TEXT_H
#define TEXT_H

extern "C" {
#include <SDL2/SDL_ttf.h>

#ifdef USE_GLES
#include <GLES2/gl2.h>
#endif
#ifdef USE_GL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif
}

#include "text_font.hpp"
#include "image.hpp"
#include "callback.hpp"



class GameWindow;



///
/// Text (box) which uses an image as its display method.
///
/// Must be passed around by reference or pointer.
///
class Text {
private:
    ///
    /// If true, the text needs to be re-rendered.
    ///
    bool dirty;
    ///
    /// The text that need to be rendered.
    ///
    std::string text;
    ///
    /// If true, the text needs to be re-rendered.
    ///
    bool smooth;
    ///
    /// The width of the textbox.
    ///
    int width;
    ///
    /// The height of the textbox.
    ///
    int height;
    ///
    /// Horizontal position of text.
    ///
    /// Displacement of left edge of text from left edge of window.
    ///
    int x;
    ///
    /// Vertical position of text.
    ///
    /// Displacement of bottom edge of text from bottom edge of window.
    ///
    int y;
    ///
    /// Image used to store the rendered text
    ///
    Image image;
    ///
    /// Texture used in opengl.
    ///
    GLuint texture;
    ///
    /// Vertex buffer object used in opengl.
    ///
    /// Stores both the vertex position and texture coordinate data for
    /// the text being displayed.
    ///
    GLuint vbo;
    ///
    /// The colour to render the text as.
    ///
    GLfloat rgba[4];
    ///
    /// Font to use.
    ///
    TextFont font;
    ///
    /// The game window to draw on.
    ///
    GameWindow* window;
    ///
    /// Callback which causes recalculation of vbo on window resize.
    ///
    Callback<void,GameWindow*> resize_callback;

    ///
    /// Re-render the text.
    ///
    void render();

    ///
    /// Create an OpenGL texture from the image.
    ///
    void generate_texture();

    ///
    /// Creates text-specific vertex buffer object.
    ///
    void generate_vbo();
public:
    ///
    /// Represents a failure when rendering or drawing.
    ///
    class RenderException: public std::exception {
    protected:
        const char* message;
    public:
        RenderException(const char* message);
        virtual const char* what() const noexcept;
    };
    
    Text(GameWindow* window, TextFont font, bool smooth = false);
    ~Text();

    ///
    /// Sets the text.
    ///
    void set_text(std::string text);

    ///
    /// Render the text to an image, and subsequently to an OpenGL
    /// texture, give size information.
    ///
    /// Width and height are given in pixels.
    ///
    void resize(int w, int h);
    ///
    /// Render the text to an image, and subsequently to an OpenGL
    /// texture, give size information.
    ///
    /// Width and height are given in screen ratios.
    ///
    void resize(float w, float h);
    ///
    /// Move the text to at the given coordinates (pixels from
    /// bottom-left).
    ///
    void move(int x, int y);
    ///
    /// Move the text to at the given coordinates (screen ratio from
    /// bottom-left).
    ///
    void move(float x, float y);
    ///
    /// Display the text.
    ///
    void display();
};



#endif

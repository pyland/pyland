#ifndef TEXT_H
#define TEXT_H

extern "C" {
#ifdef USE_GL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif

#ifdef USE_GLES
#include <GLES2/gl2.h>
#endif
}

#include <stdexcept>
#include <string>
#include <utility>

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
    enum class Alignment {
        LEFT, RIGHT, TOP, BOTTOM, CENTRE
    };
    ///
    /// If true, the text needs to be re-rendered.
    ///
    bool dirty_texture;
    ///
    /// If true, the text vbo needs to be re-generated.
    ///
    bool dirty_vbo;
    ///
    /// The text that need to be rendered.
    ///
    std::string text;
    ///
    /// Position text based on alignment.
    ///
    /// If true, rather than the position representing the top left of
    /// the text area, it will represent a position dependant on the
    /// current alignment settings. The corner/edge/centre is the
    /// alignment_v-alignment_h corner/edge/centre.
    ///
    bool position_from_alignment;
    ///
    /// The horizontal justification
    ///
    Alignment alignment_h;
    ///
    /// The vertical justification
    ///
    Alignment alignment_v;
    ///
    /// The width of the text (not textbox).
    ///
    int used_width;
    ///
    /// The height of the text (not textbox).
    ///
    int used_height;
    ///
    /// If true, the text needs to be re-rendered.
    ///
    bool smooth;
    ///
    /// The radius of the glow effect in pixels.
    ///
    /// If set to 0, glow is disabled.
    ///
    int glow_radius;
    ///
    /// The width of the textbox.
    ///
    int width;
    ///
    /// The width as a ratio if set using ratios.
    ///
    float width_ratio;
    ///
    /// The height of the textbox.
    ///
    int height;
    ///
    /// The height as a ratio if set using ratios.
    ///
    float height_ratio;
    ///
    /// Horizontal position of text.
    ///
    /// Displacement of left edge of text from left edge of window,
    /// unless position_from_alignment is set, in which case the text
    /// origin can be the left, centre, or right top corner.
    ///
    int x;
    ///
    /// The horizontal position as a ratio if set using ratios.
    ///
    float x_ratio;
    ///
    /// Vertical position of text.
    ///
    /// Displacement of bottom edge of text from bottom edge of window.
    ///
    int y;
    ///
    /// The vertical position as a ratio if set using ratios.
    ///
    float y_ratio;
    ///
    /// Whether the user used relative sizing (ratio across screen)
    ///
    bool ratio_size;
    ///
    /// Whether the user used relative positioning (ratio across screen)
    ///
    bool ratio_position;
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
    uint8_t rgba[4];
    ///
    /// The colour to render the text as.
    ///
    uint8_t glow_rgba[4];
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
    /// Applies a bloom effect to text.
    ///
    void apply_newson_bloom();

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
    class RenderException: public std::runtime_error {
    public:
        RenderException(const char  *message);
        RenderException(const std::string &message);
    };

    Text(GameWindow* window, TextFont font, bool smooth = false);
    ~Text();

    ///
    /// Sets the text.
    ///
    void set_text(std::string text);

    ///
    /// Calculates the size used by the text area.
    ///
    /// This is the size of what is actually displayed.
    ///
    std::pair<int,int> get_rendered_size();
    ///
    /// Calculates the ratio size used by the text area.
    ///
    /// This is the size of what is actually displayed.
    ///
    std::pair<float,float> get_rendered_size_ratio();

    ///
    /// Calculates the size wanted by the text.
    ///
    /// If there is too much text, this can be larger the the text area.
    ///
    std::pair<int,int> get_text_size();
    ///
    /// Calculates the ratio size wanted by the text.
    ///
    /// If there is too much text, this can be larger the the text area.
    ///
    std::pair<float,float> get_text_size_ratio();

    ///
    /// Gets the size of the text area.
    ///
    /// This is the size allocated for the text area (by resize method).
    ///
    std::pair<int,int> get_size();
    ///
    /// Gets the ratio size of the text area.
    ///
    /// This is the size allocated for the text area (by resize method).
    ///
    std::pair<float,float> get_size_ratio();

    ///
    /// Gets the origin of the text area.
    ///
    /// This is the position set for the text area (by move method).
    ///
    std::pair<int,int> get_origin();
    ///
    /// Gets the ratio origin of the text area.
    ///
    /// This is the position set for the text area (by move method).
    ///
    std::pair<float,float> get_origin_ratio();

    ///
    /// Gets the top-left corner of the text area.
    ///
    /// This is the position from the bottom-left of the window.
    ///
    std::pair<int,int> get_top_left();
    ///
    /// Gets the ratio top-left corner of the text area.
    ///
    /// This is the position from the bottom-left of the window.
    ///
    std::pair<float,float> get_top_left_ratio();

    ///
    /// Aligns the text to the left.
    ///
    void align_left();
    ///
    /// Horizontally centres text.
    ///
    void align_centre();
    ///
    /// Aligns the text to the right.
    ///
    void align_right();
    ///
    /// Aligns the text to the top.
    ///
    void vertical_align_top();
    ///
    /// Vertically centres text.
    ///
    void vertical_align_centre();
    ///
    /// Aligns the text to the bottom.
    ///
    void vertical_align_bottom();
    ///
    /// If true, the origin of the text will depend on alignment.
    ///
    /// When set to true:
    /// if alignment_h is LEFT  , the origin is the Y-left;
    /// if alignment_h is CENTRE, the origin is the Y-centre;
    /// if alignment_h is RIGHT , the origin is the Y-right;
    /// if alignment_v is TOP   , the origin is the top-X;
    /// if alignment_v is CENTRE, the origin is the centre-X;
    /// if alignment_v is BOTTOM, the origin is the bottom-X;
    ///
    void align_at_origin(bool aao);

    ///
    /// Set the colour of text.
    ///
    void set_colour(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    ///
    /// Apply a bloom effect of a certain size.
    ///
    /// @param radius The radius of the bloom effect in pixels. A value
    ///               of 0 disables bloom.
    ///
    void set_bloom_radius(int radius);
    ///
    /// Set the colour of the bloom effect.
    ///
    void set_bloom_colour(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    ///
    /// Render the text to an image, and subsequently to an OpenGL
    /// texture, give size information.
    ///
    /// Width and height are given in pixels. If a dimension is 0, then
    /// it is automatically sized.
    ///
    void resize(int w, int h);
    ///
    /// Render the text to an image, and subsequently to an OpenGL
    /// texture, give size information.
    ///
    /// Width and height are given in screen ratios. If a dimension is
    /// 0, then it is automatically sized.
    ///
    void resize_ratio(float w, float h);
    ///
    /// Move the text to at the given coordinates (pixels from
    /// bottom-left).
    ///
    void move(int x, int y);
    ///
    /// Move the text to at the given coordinates (screen ratio from
    /// bottom-left).
    ///
    void move_ratio(float x, float y);
    ///
    /// Display the text.
    ///
    void display();
};



#endif

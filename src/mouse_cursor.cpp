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


#include "mouse_cursor.hpp"

#include "game_window.hpp"
#include "input_manager.hpp"
#include "lifeline.hpp"
#include "mouse_input_event.hpp"
#include "renderable_component.hpp"
#include "shader.hpp"
#include "texture_atlas.hpp"



#define SHADER_VARIABLE_POSITION "position"
#define SHADER_VARIABLE_TEXTURE  "texture_coord"
#define SHADER_LOCATION_POSITION 0
#define SHADER_LOCATION_TEXTURE  1



MouseCursor::MouseCursor(GameWindow* window):
    window(window),
    callback_lifeline(window->get_input_manager()->register_mouse_handler(
        [this] (MouseInputEvent event) {
            x = event.to.x;
            y = event.to.y;
            dirty = true;
        })),
    atlas(TextureAtlas::get_shared("../resources/cursor.png")),
    shader(Shader::get_shared("cursor_shader")),
    vbo(0),
    x(0),
    y(0),
    dirty(true)
{
    atlas->set_tile_size(64, 64);
    glGenBuffers(1, &vbo);

    shader->bind_location_to_attribute(SHADER_LOCATION_POSITION, SHADER_VARIABLE_POSITION);
    shader->bind_location_to_attribute(SHADER_LOCATION_TEXTURE, SHADER_VARIABLE_TEXTURE);
    shader->link();

    std::tie(tex_x1, tex_x2, tex_y1, tex_y2) = atlas->index_to_coords(atlas->get_name_index("cursor/arrow"));
}

MouseCursor::~MouseCursor() {
    glDeleteBuffers(1, &vbo);
}


void MouseCursor::display() {
    window->use_context();

    glUseProgram(shader->get_program());
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, atlas->get_gl_texture());

    if (dirty) {
        std::pair<float,float> lower = window->get_ratio_from_pixels(std::make_pair(x-32, y-32));
        std::pair<float,float> upper = window->get_ratio_from_pixels(std::make_pair(x+31, y+31));
        GLfloat vbo_data[] = {
            // Triangle 1:
            //     Bottom-left  (0, 0)
            lower.first, lower.second,
            tex_x1     , tex_y1,
            //     Top-left     (0, 1)
            lower.first, upper.second,
            tex_x1    , tex_y2,
            //     Top-right    (1, 1)
            upper.first, upper.second,
            tex_x2     , tex_y2,
            // Triangle 2:
            //     Bottom-left  (0, 0)
            lower.first, lower.second,
            tex_x1     , tex_y1,
            //     Top-right    (1, 1)
            upper.first, upper.second,
            tex_x2     , tex_y2,
            //     Bottom-right (1, 0)
            upper.first, lower.second,
            tex_x2     , tex_y1
        };
        glBufferData(GL_ARRAY_BUFFER, sizeof(vbo_data), vbo_data, GL_DYNAMIC_DRAW);
        dirty = false;
    }

    // Position data.
    glVertexAttribPointer(SHADER_LOCATION_POSITION, 2, GL_FLOAT, GL_FALSE, 4 * (GLsizei)sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
    // Texture data.
    glVertexAttribPointer(SHADER_LOCATION_TEXTURE, 2, GL_FLOAT, GL_FALSE, 4 * (GLsizei)sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(SHADER_LOCATION_POSITION);
    glEnableVertexAttribArray(SHADER_LOCATION_TEXTURE);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glEnable(GL_DEPTH_TEST);
}

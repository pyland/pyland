#include <cstring>
#include <cstdio>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>

#include <glog/logging.h>

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
#include "shader.hpp"
#include "text.hpp"
#include "game_window.hpp"
#include "callback.hpp"



// NOTE:
//   Text can be rendered less directly by defining
//   TEXT_SAFE_SURFACE. This will copy the surface into another surface
//   of an explicitly defined format - which is slow, but is safer
//   use. In theory, disabling this also works perfectly though, giving
//   a performance boost by directly reading off the text.



#define SHADER_VARIABLE_POSITION "position"
#define SHADER_VARIABLE_TEXTURE  "texture_coord"
#define SHADER_VARIABLE_COLOUR   "colour"
#define SHADER_LOCATION_POSITION 0
#define SHADER_LOCATION_TEXTURE  1
#define SHADER_LOCATION_COLOUR   2



///
/// Shader programs used in opengl.
///
/// Each GL context (per window) must have a shader.
///
/// WARNING: Potential memory leak when windows are destructed.
///
static std::map<GameWindow*, std::shared_ptr<Shader>> shaders;



static void load_program(GameWindow* window) {
    window->use_context();
#ifdef USE_GLES
    shaders.insert(std::make_pair(window,std::make_shared<Shader>("text_shader.glesv", "text_shader.glesf")));
#endif
#ifdef USE_GL
    shaders.insert(std::make_pair(window,std::make_shared<Shader>("text_shader.glv", "text_shader.glf")));
#endif
    std::shared_ptr<Shader> shader = shaders.find(window)->second;
    shader->bind_location_to_attribute(SHADER_LOCATION_POSITION, SHADER_VARIABLE_POSITION);
    shader->bind_location_to_attribute(SHADER_LOCATION_TEXTURE, SHADER_VARIABLE_TEXTURE);
    shader->bind_location_to_attribute(SHADER_LOCATION_COLOUR, SHADER_VARIABLE_COLOUR);
    shader->link();
}



// Need to inherit constructors manually.
// NOTE: This will, and are required to, copy the message.
Text::RenderException::RenderException(const char *message): std::runtime_error(message) {}
Text::RenderException::RenderException(const std::string &message): std::runtime_error(message) {}


Text::Text(GameWindow* window, TextFont font, bool smooth):
    dirty(true),
    text(""),
    smooth(smooth),
    texture(0),
    vbo(0),
    font(font),
    window(window),
    resize_callback([this] (GameWindow*) {this->dirty = true;}) {
        rgba[0] = rgba[1] = rgba[2] = rgba[3] = 1.0f;
        window->register_resize_handler(resize_callback);
}

Text::~Text() {
    resize_callback.unregister_everywhere();
    glDeleteTextures(1, &texture);
    glDeleteBuffers(1, &vbo);
}


void Text::render() {
    int line_height = TTF_FontHeight(font.font);
    int line_number = 0;
    int lost_lines = 0;

    int length = (int)text.length();
    // Entire text.
    const char* ctext = text.c_str();
    
    image = Image(width, height, true);
    
    // Line of text.
    char* line = new char[length];

    image.clear(0x00000000, 0xffffffff);
    // Text indexing.
    for (int t = 0; t < length; t++) {
        int line_width;
        // Word indexing.
        int w = 0;
        // Stores the (from the beginning) a word in ctext (copy).
        line[0] = '\0';
        // Points to the end of the last successfully fitting word in
        // line.
        int ll = 0;

        // Line indexing.
        for (int l = t; l < length; l++) {
            // Find word end.
            for (w = l; w < length && (ctext[w] != ' ' && ctext[w] != '\n'); w++);
            // Copy word into line.
            for (; l < w; l++) {
                line[l - t] = ctext[l];
            }
            line[l - t] = '\0';
            // Test line length.
            TTF_SizeUTF8(font.font, line, &line_width, NULL);
            if (line_width < width) {
                // Mark current position as valid.
                ll = l - t;
                // If this is the end of a line, or whole text, stop.
                if (ctext[w] == '\n' || ctext[w] == '\0') {
                    t = l;
                    // Note the string is already null terminated.
                    break;
                }
                else {
                    // Replace null terminator with word separator.
                    line[ll] = ctext[w];
                }
            }
            else {
                if (ll > 0) {
                    // There are some fittable words.
                    // As the line overflows with the current l, set the
                    // text index to the last fit.
                    t += ll;
                }
                else {
                    // There are no separators to break on.
                    // We're going to have to cut a word in half.

                    // Swapped-out character
                    char c;
                    int left = 0;
                    int right = l - t;
                    // Search index within line.
                    int ls;
                    ll = 0;
                    for (ls = (right + left) / 2; ls != ll; ls = (right + left) / 2) {
                        ll = ls;
                        c = line[ls];
                        line[ls] = 0;
                        TTF_SizeUTF8(font.font, line, &line_width, NULL);
                        if (line_width <= width) {
                            left = ls;
                        }
                        else {
                            right = ls;
                        }
                        line[ls] = c;
                    }
                    // Don't skip the non-separating character.
                    t += ll - 1;
                }
                // Null terminate for rendering.
                line[ll] = '\0';
                break;
            }
        }
        if (line[0] == '\0') {
            // Check that we aren't going to chase our tailes trying to fit an unfittable character.
            if (ctext[w] != '\n') {
                LOG(WARNING) << "Cannot render text: character too large.";
                delete line;
                throw Text::RenderException("A word is too long");
            }
            else {
                // It's a blank line.
                line_number++;
                continue;
            }
        }
        // Render line
        SDL_Color black;
        black.r = black.g = black.b = black.a = 0;
        SDL_Color white;
        white.r = white.g = white.b = white.a = 255;
        LOG(INFO) << "Rendering line of text: \"" << line << "\".";
        
        SDL_Surface* rendered_line;
        if (smooth) {
            rendered_line = TTF_RenderUTF8_Shaded(font.font, line, white, black);
        }
        else {
            rendered_line = TTF_RenderUTF8_Solid(font.font, line, white);
        }
#ifdef TEXT_SAFE_SURFACE
        // This surface has a known format.
        SDL_Surface* compatible = SDL_CreateRGBSurface(0, // Unsed
                                                       rendered_line->w,
                                                       rendered_line->h,
                                                       32, // 32 bit works well, others... don't.
                                                       0x00,
                                                       0x00,
                                                       0x00,
                                                       0xff
                                                       );
        SDL_FillRect(compatible, NULL, 0);
        SDL_SetSurfaceBlendMode(rendered_line, SDL_BLENDMODE_NONE);
        SDL_BlitSurface(rendered_line, NULL, compatible, NULL);
        SDL_LockSurface(compatible);
        // pitch is in bytes, not pixels.
        int jump = compatible->pitch / 4;
#else
        SDL_LockSurface(rendered_line);
        int jump = rendered_line->pitch;
#endif
        // "What is this pointless copy?" you might ask.
        //   Well, it is a suggestion to the c++ compiler that smooth
        //   will not change throughout this function, so it can
        //   optimise the if containing _smooth out of the for loop
        //   entirely, rather than checking it ever single iteration.
        int _smooth = smooth;
        for (int y = 0; y < line_height; y++) {
            int yi = y + line_number * line_height;
            if (yi >= image.height) {
                lost_lines++;
                break;
            }
            for (int x = 0; x < rendered_line->w; x++) {
#ifdef TEXT_SAFE_SURFACE
                image.flipped_pixels[yi][x].a = (((Uint32*)compatible->pixels)[(y*jump + x)]);
#else
                if (_smooth) {
                    image.flipped_pixels[yi][x].a =(((Uint8*)rendered_line->pixels)[(y*jump + x)]);
                }
                else {
                    image.flipped_pixels[yi][x].a = (((Uint8*)rendered_line->pixels)[(y*jump + x)]) ? 255 : 0;
                }
#endif
            }
        }
#ifdef TEXT_SAFE_SURFACE
        SDL_UnlockSurface(compatible);
        SDL_FreeSurface(compatible);
#else
        SDL_UnlockSurface(rendered_line);
#endif
        SDL_FreeSurface(rendered_line);
        line_number++;
        if (y < line_height) {
            LOG(WARNING) << "Text overflow.";
            break;
        }
    }
    delete[] line;
    generate_texture();
    dirty = false;
}


void Text::generate_texture() {
    if (texture != 0) {
        glDeleteTextures(1, &texture);
    }

    glGenTextures(1, &texture);

    if (texture == 0) {
        LOG(ERROR) << "Error rendering text: Unable to generate GL texture.";
        throw Text::RenderException("Unable to generate GL texture");
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
#ifdef USE_GL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, image.store_width, image.store_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.pixels);
#endif
#ifdef USE_GLES
    // For some STUPID reason, GL ES seems to be forcing me to waster
    // RGB channels for padding, on a system with little memory.
    // Look into this later.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.store_width, image.store_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.pixels);
#endif
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}


void Text::generate_vbo() {
    if (dirty) {
        return;
    }
    if (vbo == 0) {
        // Create VBO.
        glGenBuffers(1, &vbo);
        if (vbo == 0) {
            LOG(ERROR) << "Error rendering text: Unable to generate GL VBO.";
            throw Text::RenderException("Unable to generate GL VBO");
        }
    }

    std::pair<float, float> ratio_xy = window->get_ratio_from_pixels(std::pair<int,int>(x, y));
    std::pair<float, float> ratio_wh = window->get_ratio_from_pixels(std::pair<int,int>(width, height));
    // We are working with opengl coordinates where we strech from -1.0
    // to 1.0 across the window.
    float rx = ratio_xy.first * 2.0f - 1.0f;
    float ry = ratio_xy.second * 2.0f - 1.0f;
    float rw = ratio_wh.first * 2.0f;
    float rh = ratio_wh.second * 2.0f;
    float tw = (float)image.width  / (float)image.store_width;
    float th = 1.0f - ((float)image.height / (float)image.store_height);
    
    // float rx = -1.0f;
    // float ry = -1.0f;
    // float rw = 2.0f;
    // float rh = 2.0f;
    // float tw = 1.0f;
    // float th = 1.0f;
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // There are 4 vertices in a rectangle, but we use 6 vertices in 2
    // triangles to make the rectangle.
    // Each vertex has 2 floats for position, and 2 floats for texture
    // coordinates.
    // Format: vertex_x, vertex_y, texture_x, texture_y, ...
    GLfloat vbo_data[] = {
        // Triangle 1:
        //     Bottom-left  (0, 0)
        rx     , ry - rh, 0.0f, th  ,
        //     Top-left     (0, 1)
        rx     , ry     , 0.0f, 1.0f,
        //     Top-right    (1, 1)
        rx + rw, ry     , tw  , 1.0f,
        // Triangle 2:
        //     Bottom-left  (0, 0)
        rx     , ry - rh, 0.0f, th  ,
        //     Top-right    (1, 1)
        rx + rw, ry     , tw  , 1.0f,
        //     Bottom-right (1, 0)
        rx + rw, ry - rh, tw  , th  ,
    };
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(vbo_data), vbo_data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void Text::set_text(std::string text) {
    this->text = text;
    dirty = true;
}


void Text::resize(int w, int h) {
    if (width != w || height != h) {
        width = w;
        height = h;
        dirty = true;
    }
}

void Text::resize(float w, float h) {
    int iw, ih;
    std::pair<int,int> window_size = window->get_size();
    iw = (int)(w * (float)window_size.first);
    ih = (int)(h * (float)window_size.second);
    if (width != iw || height != ih) {
        width = iw;
        height = ih;
        dirty = true;
    }
}


void Text::move(int x, int y) {
    if (this->x != x || this->y != y) {
        this->x = x;
        this->y = y;
        generate_vbo();
    }
    // std::pair<int,int> window_size = window->get_size();
    // display((float)x / (float)window_size.first,
    //         (float)y / (float)window_size.second,
    //         (float)(x + width) / (float)window_size.first,
    //         (float)(y + height) / (float)window_size.second);
}

void Text::move(float x, float y) {
    int ix, iy;
    std::pair<int,int> window_size = window->get_size();
    ix = (int)(x * (float)window_size.first);
    iy = (int)(y * (float)window_size.second);
    if (this->x != ix || this->y != iy) {
        this->x = ix;
        this->y = iy;
        generate_vbo();
    }
    // std::pair<int,int> window_size = window->get_size();
    // display(x,
    //         y,
    //         x + ((float)(width) / (float)window_size.first),
    //         y + ((float)(height) / (float)window_size.second));
}

void Text::display() {
    window->use_context();
    if (dirty) {
        try {
            render();
            generate_vbo();
        }
        catch (Text::RenderException e) {
            LOG(WARNING) << e.what();
            return;
        }
    }
    if (shaders.count(window) == 0) {
        load_program(window);
    }
    if (vbo == 0) {
        // Assume that the size has not been initialized.
        return;
    }

    std::shared_ptr<Shader> shader = shaders.find(window)->second;
    glUseProgram(shader->get_program());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glDisable(GL_DEPTH_TEST);

    // Position data.
    glVertexAttribPointer(SHADER_LOCATION_POSITION, 2, GL_FLOAT, GL_FALSE, 4 * (GLsizei)sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
    // Texture data.
    glVertexAttribPointer(SHADER_LOCATION_TEXTURE, 2, GL_FLOAT, GL_FALSE, 4 * (GLsizei)sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
    // glBindAttriLocation(program, PROGRAM_LOCATION_POSITION, SHADER_VARIABLE_POSITION);
    // glBindAttriLocation(program, PROGRAM_LOCATION_TEXTURE , SHADER_VARIABLE_TEXTURE);
    glUniform4fv(glGetUniformLocation(shader->get_program(), "colour"), 1, rgba);
    // glUniform4fv(SHADER_LOCATION_COLOUR, 1, rgba);
    glEnableVertexAttribArray(SHADER_LOCATION_POSITION);
    glEnableVertexAttribArray(SHADER_LOCATION_TEXTURE);
    glDisableVertexAttribArray(SHADER_LOCATION_COLOUR);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
    glEnable(GL_DEPTH_TEST);
}

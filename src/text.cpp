// //////////////////////////////////////////////////////////////
// //////////////////////// CURRENT BUGS ////////////////////////
// //////////////////////////////////////////////////////////////
// Possible SDL_ttf bug when rendering certain first characters.
//      Workaround is to append and prepend a space character to lines.
//      See render(): border, safe, safe_str.
//

// Behaviour modifiers (defines):
//  TEXT_SAFE_SURFACE
//      Text can be rendered less directly by defining this. This will
//      copy the surface into another surface of an explicitly defined
//      format - which is slow, but is safer use. In theory, disabling
//      this also works perfectly though, giving a performance boost by
//      directly reading off the text.

#include <cmath>
#include <cstdio>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>

#include <glog/logging.h>

#include "callback.hpp"f
#include "game_window.hpp"
#include "image.hpp"
#include "shader.hpp"
#include "text.hpp"
#include "text_font.hpp"



#define SHADER_VARIABLE_POSITION "position"
#define SHADER_VARIABLE_TEXTURE  "texture_coord"
#define SHADER_LOCATION_POSITION 0
#define SHADER_LOCATION_TEXTURE  1



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
    // shader->bind_location_to_attribute(SHADER_LOCATION_COLOUR, SHADER_VARIABLE_COLOUR);
    shader->link();
}



// Need to inherit constructors manually.
// NOTE: This will, and are required to, copy the message.
Text::RenderException::RenderException(const char *message): std::runtime_error(message) {}
Text::RenderException::RenderException(const std::string &message): std::runtime_error(message) {}


Text::Text(GameWindow* window, TextFont font, bool smooth):
    dirty_texture(true),
    dirty_vbo(true),
    text(""),
    position_from_alignment(false),
    alignment_h(Text::Alignment::LEFT),
    alignment_v(Text::Alignment::TOP),
    smooth(smooth),
    glow_radius(0),
    width(0),
    width_ratio(0),
    height(0),
    height_ratio(0),
    x(0),
    x_ratio(0),
    y(0),
    y_ratio(0),
    ratio_size(false),
    ratio_position(true),
    texture(0),
    vbo(0),
    font(font),
    window(window),
    resize_callback([this] (GameWindow*) {
            dirty_vbo = true;
            if (ratio_position) {
                move_ratio(x_ratio, y_ratio);
            }
            if (ratio_size) {
                resize_ratio(width_ratio, height_ratio);
            }
        })
{
    rgba[0] = rgba[1] = rgba[2] = rgba[3] = 255;
    glow_rgba[0] = glow_rgba[1] = glow_rgba[2] = 0;
    glow_rgba[3] = 255;
    window->register_resize_handler(resize_callback);
}

Text::~Text() {
    resize_callback.unregister_everywhere();
    glDeleteTextures(1, &texture);
    glDeleteBuffers(1, &vbo);
}


// WARNING:
//      This function contains an unusual amount of raw pointers, and is
//      the most C-like function you can get without being C++.
//      Sorry Joshua.
void Text::render() {
    int width = this->width;
    int height = this->height;
    std::pair<int,int> window_size = window->get_size();
    // Automatic sizing if dimension is 0.
    if (width == 0) {
        switch (alignment_h) {
        default:
        case Alignment::LEFT:
            width = window_size.first - this->x;
            break;
        case Alignment::CENTRE:
            width = ((window_size.first / 2) < this->x) ? this->x
                                                        : window_size.first - this->x;
            break;
        case Alignment::RIGHT:
            width = this->x;
            break;
        }
    }
    if (height == 0) {
        switch (alignment_v) {
        default:
        case Alignment::TOP:
            height = this->y;
            break;
        case Alignment::CENTRE:
            height = ((window_size.second / 2) < this->y) ? window_size.second - this->y
                                                          : this->y;
            break;
        case Alignment::BOTTOM:
            height = window_size.second - this->y;
            break;
        }
    }

    // If they are still zero, don't continue.
    if (width == 0 || height == 0) {
        throw Text::RenderException("Invalid dimensions after auto-sizing.");
    }

    int available_width = width - glow_radius * 2;
    // It took a whole day to discover that there was a bug in
    // SDL_ttf. Starting with certain characters on certain
    // fonts seems to break it. :(
    // As a hack, prepend and (for balance) append a space.
    int border;
    TTF_SizeUTF8(font.font, " ", &border, NULL);
    border *= 2;

    // If they are still zero, don't continue.
    if (available_width <= 0) {
        throw Text::RenderException("No available width for rendering text.");
    }

    // int available_height = height - glow_radius * 2;
    int line_height = TTF_FontHeight(font.font);
    int line_number = 0;
    int lost_lines = 0;

    int used_width = 0;

    int length = (int)text.length();
    // Entire text.
    const char* ctext = text.c_str();

    // Line of text.
    char* line = new char[length+1];

    // Null-terminator separated lines of text.
    // The worst case is a character per line, so we need to do
    // 2 * length (character, null, character, null, ...).
    char* lines = new char[2*length+1];
    // Pointer to within lines.
    char* lines_scan = lines;

    // Text indexing.
    for (int t = 0; t < length; t++) {
        int line_width = 0;
        // Word indexing.
        int w = 0;
        // Stores (from the beginning) a word in ctext (copy).
        line[0] = '\0';
        // Points to the end of the last successfully fitting word in
        // line.
        int ll = 0;

        // Line indexing.
        for (int l = t; l < length; l++) {
            // Find word end.
            for (w = l; w < length && (ctext[w] != ' ' && ctext[w] != '\n'); w++) {
                // As we are dealing with UTF-8, we must make sure to
                // copy an entire character if it is more than one byte.
                // The bit formats of UTF-8 characters are:
                // 0xxxxxxx
                // 110xxxxx 10xxxxxx
                // 1110xxxx 10xxxxxx 10xxxxxx
                // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
                // 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
                // 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
                //
                // ... Well, actually it's not THAT simple, because
                // there are these magical characters which modify other
                // characters... But it's not worth the effort.
                if (ctext[l] & 0x80) {
                    // The number of bits in the first byte set to 1
                    // before the first zero indicate the total number
                    // of bytes to use for the UTF-8 character.
                    for (int bits = ctext[w] << 1; w < length && (bits & 0x80); w++, bits <<= 1);
                }
            }
            // Copy word into line.
            for (; l < w; l++) {
                line[l - t] = ctext[l];
            }
            line[l - t] = '\0';
            // Test line length.
            TTF_SizeUTF8(font.font, line, &line_width, NULL);
            // Part of SDL_ttf bug workaround.
            line_width+=border;
            if (line_width <= available_width) {
                if (line_width > used_width) {
                    used_width = line_width;
                }
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
                        // Part of SDL_ttf bug workaround.
                        line_width+=border;
                        if (line_width <= available_width) {
                            if (line_width > used_width) {
                                used_width = line_width;
                            }
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

        // Copy the line into lines (using lines_scan).
        int i;
        for (i = 0; line[i] != '\0'; ++i) {
            lines_scan[i] = line[i];
        }
        lines_scan[i] = '\0';
        lines_scan = &lines_scan[i+1];

        ++line_number;
        if (line[0] == '\0') {
            // Check that we aren't going to chase our tailes trying to fit an unfittable character.
            if (ctext[w] != '\n') {
                LOG(WARNING) << "Cannot render text: character too large.";
                delete[] line;
                delete[] lines;
                throw Text::RenderException("A character is too large");
            }
            else {
                // It's a blank line.
                continue;
            }
        }
    }
    int line_count = line_number;

    int used_height = line_count * line_height + 2 * glow_radius;

    used_width += glow_radius * 2 + border;

    image = Image(used_width, (used_height < height) ? used_height : height, true);
    uint8_t clear_colour[4] = {rgba[0], rgba[1], rgba[2], 0x00};
    uint8_t clear_mask[4] = {0xff, 0xff, 0xff, 0xff};
    image.clear(clear_colour, clear_mask);
    // image.clear(0xffffff00, 0xffffffff);

    // Render all lines of text.
    SDL_Color blank;
    blank.r = blank.g = blank.b = blank.a = 0;
    SDL_Color colour;
    colour.r = rgba[0];
    colour.g = rgba[1];
    colour.b = rgba[2];
    colour.a = rgba[3];
    lines_scan = lines;
    for (int line_number = 0; line_number < line_count; ++line_number) {
        // Render line
        VLOG(2) << "Rendering line of text: \"" << lines_scan << "\".";
        if (lines_scan[0] == '\0') {
            // Skip it - it's a new line.
            lines_scan = &lines_scan[1];
            continue;
        }

        SDL_Surface* rendered_line;
        {
            // It took a whole day to discover that there was a bug in
            // SDL_ttf. Starting with certain characters on certain
            // fonts seems to break it. :(
            // As a hack, prepend and (for balance) append a space.
            std::stringstream safe;
            safe << " " << lines_scan << " ";
            std::string safe_str(safe.str());
            if (smooth) {
                rendered_line = TTF_RenderUTF8_Shaded(font.font, safe_str.c_str(), colour, blank);
            }
            else {
                rendered_line = TTF_RenderUTF8_Solid(font.font, safe_str.c_str(), colour);
            }
        }

        if (rendered_line == nullptr) {
            LOG(WARNING) << "Cannot render line of text: \"" << lines_scan << "\".";
            delete[] line;
            delete[] lines;
            throw Text::RenderException("Cannot render line of text");
        }

#ifdef TEXT_SAFE_SURFACE
        // This surface has a known format.
        SDL_Surface* compatible = SDL_CreateRGBSurface(0, // Unsed
                                                       rendered_line->w,
                                                       rendered_line->h,
                                                       32,
                                                       0x00,
                                                       0x00,
                                                       0x00,
                                                       0xff
                                                       );
        SDL_FillRect(compatible, NULL, 0);
        SDL_SetSurfaceBlendMode(rendered_line, SDL_BLENDMODE_NONE);
        SDL_BlitSurface(rendered_line, NULL, compatible, NULL);
        SDL_LockSurface(compatible);
        // pitch is in bytes, not pixels. RGBA = 4 bytes.
        int jump = compatible->pitch / 4;
#else
        SDL_LockSurface(rendered_line);
        int jump = rendered_line->pitch;
#endif
        // "What is this pointless copy?" you might ask.
        //   Well, it is a suggestion to the c++ compiler that smooth
        //   will not change throughout this function, so it can
        //   optimise the if containing _smooth out of the for loop
        //   entirely, rather than checking it every single iteration.
        int _smooth = smooth;
        int x_offset;
        int y_offset;
        switch (alignment_h) {
        default:
        case Alignment::LEFT:
            x_offset = glow_radius;
            break;
        case Alignment::CENTRE:
            x_offset = (used_width - rendered_line->w) / 2;
            break;
        case Alignment::RIGHT:
            x_offset = used_width - rendered_line->w - glow_radius;
            break;
        }
        switch (alignment_v) {
        default:
        case Alignment::TOP:
            y_offset = line_number * line_height;
            break;
        case Alignment::CENTRE:
            y_offset = line_number * line_height - (used_height - image.height) / 2;
            break;
        case Alignment::BOTTOM:
            y_offset = line_number * line_height - (used_height - image.height);
            break;
        }
        y_offset += glow_radius;
        // x surface
        int xs;
        // y surface
        int ys;
        for (ys = 0; ys < rendered_line->h; ++ys) {
            int yi = ys + y_offset;
            if (yi >= image.height) {
                lost_lines++;
                break;
            }
            else if (yi < 0) {
                continue;
            }
            int begin_xs((x_offset >= 0) ? 0 : -x_offset);
            int end_xs((rendered_line->w < image.width - x_offset) ? rendered_line->w : image.width - x_offset);
            for (xs = begin_xs; xs < end_xs; ++xs) {
#ifdef TEXT_SAFE_SURFACE
                image.flipped_pixels[yi][xs + x_offset].a = (((Uint32*)compatible->pixels)[(ys*jump + xs)]);
#else
                if (_smooth) {
                    image.flipped_pixels[yi][xs + x_offset].a = (((Uint8*)rendered_line->pixels)[(ys*jump + xs)]);
                }
                else {
                    image.flipped_pixels[yi][xs + x_offset].a = (((Uint8*)rendered_line->pixels)[(ys*jump + xs)]) ? 255 : 0;
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
        if (ys < line_height) {
            LOG(WARNING) << "Text overflow.";
            break;
        }

        // Set lines_scan to start next line
        while (lines_scan[0] != '\0') {
            lines_scan = &lines_scan[1];
        }
        lines_scan = &lines_scan[1];
    }

    this->used_width  = used_width;
    this->used_height = used_height;
    delete[] line;
    delete[] lines;
    if (glow_radius > 0) {
        apply_newson_bloom();
    }
    generate_texture();
    dirty_texture = false;
    dirty_vbo = true;
}


// My own spicy algorithm for creating a cheap bloom effect.
//
// Perform a vertical scan, to create a list of vertical distances from
// seed points.
// Perform a horizontal scan and calculate a winning radiance from the
// distances in the vertical scan.
//
// We perform vertical scan and then horizontal to enable better
// optimisation.
void Text::apply_newson_bloom() {
    // Stores distance information from the first pass.
    int* vertical_scan(new int[image.width*image.height]);
    int width = image.width;
    int height = image.height;
    int grpo = glow_radius + 1;

    // Vertical Scan.
    int i = 0;
    for (int x = 0; x < width; ++x) {
        int seed_y = 0;
        for (; seed_y < height && image[seed_y][x].a == 0; ++seed_y);
        if (seed_y == height) {
            // It's a blank line, so move on.
            for (int y = 0; y < height; ++y) {
                vertical_scan[i] = 0;
                i += width;
            }
        } else {
            int seed_y_prev = -1;
            for (int y = 0; y < height; ++y) {
                if (y == seed_y) {
                    // Find the next seed (or go outside of image).
                    for (++seed_y; seed_y < height && image[seed_y][x].a == 0; ++seed_y);
                    seed_y_prev = y;
                }

                int r(0);
                if (seed_y_prev != -1) {
                    // Calculate radius from distance from last seed.
                    r = glow_radius - (y - seed_y_prev);
                }
                if (seed_y != height) {
                    // Calculate radius from distance from next seed.
                    int r2(glow_radius - (seed_y - y));
                    if (r2 > r) {
                        r = r2;
                    }
                }
                if (r > 0) {
                    vertical_scan[i] = r;
                }
                else {
                    // If it's too far away, set radiance to 0.
                    vertical_scan[i] = 0;
                }
                i += width;
            }
        }
        i += 1 - (width * height);
    }

    // Pre-compute pythagoras's theorem and 0-255 scale strength.
    int* pythag(new int[(grpo)*(grpo)]);
    for (int y = 0; y <= glow_radius; ++y) {
        int ry = glow_radius - y;
        for (int x = 0; x <= glow_radius; ++x) {
            int rx = x;
            int score(int((float(glow_radius) - sqrt(float(rx*rx + ry*ry))) * 255.0f / float(glow_radius)));
            pythag[x+y*grpo] = (score > 0) ? score*score/255 : 0;
        }
    }

    // Pre-compute merge colours.
    uint8_t merge_rgba[4][256];
    for (int a = 0; a < 256; ++a) {
        merge_rgba[0][a] = uint8_t(((255-a) * glow_rgba[0] + a * rgba[0]) / 255);
        merge_rgba[1][a] = uint8_t(((255-a) * glow_rgba[1] + a * rgba[1]) / 255);
        merge_rgba[2][a] = uint8_t(((255-a) * glow_rgba[2] + a * rgba[2]) / 255);
        merge_rgba[3][a] = uint8_t(((255-a) * glow_rgba[3] + a * rgba[3]) / 255);
    }

    // Pre-compute radiance alpha.
    uint8_t radiance[256];
    for (int r = 0; r < 256; ++r) {
        radiance[r] = uint8_t(int(glow_rgba[3]) * r / 255);
    }
    // uint8_t* radiance(new uint8_t[glow_radius+1]);
    // for (int r = 0; r <= glow_radius; ++r) {
    //     radiance[r] = uint8_t(int(glow_rgba[3]) * r / glow_radius);
    // }

    // Horizontal Scan.
    i = 0;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int winner = pythag[vertical_scan[i]*grpo];

            // Don't go over solid text.
            if (image[y][x].a == 0) {
                for (int dx = 1; dx < glow_radius; ++dx) {
                    int candidate(0);
                    if (x - dx >= 0) {
                        candidate = pythag[dx+vertical_scan[i - dx]*grpo];
                    }
                    if (x + dx < width) {
                        int candidate2(pythag[dx+vertical_scan[i + dx]*grpo]);
                        if (candidate2 > candidate) {
                            candidate = candidate2;
                        }
                    }
                    if (candidate > winner) {
                        winner = candidate;
                    }
                }
                image[y][x].r = glow_rgba[0];
                image[y][x].g = glow_rgba[1];
                image[y][x].b = glow_rgba[2];
                image[y][x].a = radiance[winner];
            }
            else {
                    image[y][x].r = merge_rgba[0][image[y][x].a];
                    image[y][x].g = merge_rgba[1][image[y][x].a];
                    image[y][x].b = merge_rgba[2][image[y][x].a];
                    image[y][x].a = merge_rgba[3][image[y][x].a];
            }
            ++i;
        }
    }

    delete[] vertical_scan;
    delete[] pythag;
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
    // For some STUPID reason, GL ES seems to be forcing me to waster
    // RGB channels for padding, on a system with little memory.
    // Look into this later.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.store_width, image.store_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.pixels);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
}


void Text::generate_vbo() {
    if (dirty_texture) {
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

    std::pair<float, float> ratio_xy = get_top_left_ratio();
    std::pair<float, float> ratio_wh = get_rendered_size_ratio();
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

    dirty_vbo = false;
}


void Text::set_text(std::string text) {
    this->text = text;
    dirty_texture = true;
}


std::pair<int,int> Text::get_rendered_size() {
    // Needed to update image size.
    if (dirty_texture) {
        render();
    }

    return std::make_pair(image.width, image.height);
}

std::pair<float,float> Text::get_rendered_size_ratio() {
    return window->get_ratio_from_pixels(get_rendered_size());
}


std::pair<int,int> Text::get_text_size() {
    // Needed to update image size.
    if (dirty_texture) {
        render();
    }

    return std::make_pair(used_width, used_height);
}

std::pair<float,float> Text::get_text_size_ratio() {
    return window->get_ratio_from_pixels(get_text_size());
}


std::pair<int,int> Text::get_size() {
    return std::make_pair(width, height);
}

std::pair<float,float> Text::get_size_ratio() {
    return window->get_ratio_from_pixels(get_size());
}

std::pair<int,int> Text::get_origin() {
    return std::make_pair(x, y);
}

std::pair<float,float> Text::get_origin_ratio() {
    return window->get_ratio_from_pixels(get_origin());
}

std::pair<int,int> Text::get_top_left() {
    // Needed to update image size.
    if (dirty_texture) {
        render();
    }

    int x_final;
    if (position_from_alignment) {
        switch (alignment_h) {
        default:
        case Alignment::LEFT:
            x_final = x;
            break;
        case Alignment::CENTRE:
            x_final = x - (image.width / 2);
            break;
        case Alignment::RIGHT:
            x_final = x - image.width;
            break;
        }
    } else {
        x_final = x;
    }

    int y_final;
    if (position_from_alignment) {
        switch (alignment_v) {
        default:
        case Alignment::TOP:
            y_final = y;
            break;
        case Alignment::CENTRE:
            y_final = y + (image.height / 2);
            break;
        case Alignment::BOTTOM:
            y_final = y + image.height;
            break;
        }
    } else {
        y_final = y;
    }

    return std::make_pair(x_final, y_final);
}

std::pair<float,float> Text::get_top_left_ratio() {
    return window->get_ratio_from_pixels(get_top_left());
}


void Text::align_left() {
    if (alignment_h != Alignment::LEFT) {
        alignment_h = Alignment::LEFT;
        dirty_texture = true;
    }
}

void Text::align_centre() {
    if (alignment_h != Alignment::CENTRE) {
        alignment_h = Alignment::CENTRE;
        dirty_texture = true;
    }
}

void Text::align_right() {
    if (alignment_h != Alignment::RIGHT) {
        alignment_h = Alignment::RIGHT;
        dirty_texture = true;
    }
}

void Text::vertical_align_top() {
    if (alignment_v != Alignment::TOP) {
        alignment_v = Alignment::TOP;
        dirty_texture = true;
    }
}

void Text::vertical_align_centre() {
    if (alignment_v != Alignment::CENTRE) {
        alignment_v = Alignment::CENTRE;
        dirty_texture = true;
    }
}

void Text::vertical_align_bottom() {
    if (alignment_v != Alignment::BOTTOM) {
        alignment_v = Alignment::BOTTOM;
        dirty_texture = true;
    }
}


void Text::align_at_origin(bool aao) {
    if (aao != position_from_alignment) {
        position_from_alignment = aao;
        dirty_vbo = true;
    }
}


void Text::set_colour(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    rgba[0] = r;
    rgba[1] = g;
    rgba[2] = b;
    rgba[3] = a;
    dirty_texture = true;
}


void Text::set_bloom_radius(int radius) {
    glow_radius = (radius >= 0) ? radius : 0;
    dirty_texture = true;
}


void Text::set_bloom_colour(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    glow_rgba[0] = r;
    glow_rgba[1] = g;
    glow_rgba[2] = b;
    glow_rgba[3] = a;
    dirty_texture = true;
}


void Text::resize(int w, int h) {
    if (w < 0 || h < 0) {
        std::stringstream e;
        e << "Invalid dimensions to resize (" << w << ", " << h << ")";
        throw Text::RenderException(e.str());
    }

    ratio_size = false;

    if (width != w || height != h) {
        width = w;
        height = h;
        dirty_texture = true;
        dirty_vbo = true;
    }
}

void Text::resize_ratio(float w, float h) {
    if (w < 0 || h < 0) {
        std::stringstream e;
        e << "Invalid dimensions to resize ratio (" << w << ", " << h << ")";
        throw Text::RenderException(e.str());
    }

    ratio_size = true;
    width_ratio  = w;
    height_ratio = h;

    int iw, ih;
    std::pair<int,int> window_size = window->get_size();
    iw = (int)(w * (float)window_size.first);
    ih = (int)(h * (float)window_size.second);
    if (width != iw || height != ih) {
        width = iw;
        height = ih;
        dirty_texture = true;
        dirty_vbo = true;
    }
}


void Text::move(int x, int y) {
    ratio_position = false;

    if (this->x != x || this->y != y) {
        dirty_vbo = true;
    }
    this->x = x;
    this->y = y;
}

void Text::move_ratio(float x, float y) {
    ratio_position = true;
    x_ratio = x;
    y_ratio = y;

    int ix, iy;
    std::pair<int,int> window_size = window->get_size();
    ix = (int)(x * (float)window_size.first);
    iy = (int)(y * (float)window_size.second);
    if (this->x != ix || this->y != iy) {
        this->x = ix;
        this->y = iy;
        dirty_vbo = true;
    }
}

void Text::display() {
    window->use_context();
    if (dirty_texture) {
        try {
            render();
        }
        catch (Text::RenderException e) {
            LOG(WARNING) << e.what();
            return;
        }
    }
    if (dirty_vbo) {
        try {
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
    glEnableVertexAttribArray(SHADER_LOCATION_POSITION);
    glEnableVertexAttribArray(SHADER_LOCATION_TEXTURE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
    glEnable(GL_DEPTH_TEST);
}

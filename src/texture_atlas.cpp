// Try funky initialization in if.

#include <exception>
#include <fstream>
#include <glog/logging.h>
#include <memory>
#include <set>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

extern "C" {
#ifdef USE_GL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif

#ifdef USE_GLES
#include <GLES2/gl2.h>
#endif
}

#include "cacheable_resource.hpp"
#include "engine.hpp"
#include "fml.hpp"
#include "image.hpp"
#include "resource_cache.hpp"
#include "texture_atlas.hpp"



bool TextureAtlas::global_name_to_tileset_initialized = false;
std::map<std::string, std::string> TextureAtlas::global_name_to_tileset;

std::map<std::string, std::string> const &TextureAtlas::names_to_tilesets() {
    if (!global_name_to_tileset_initialized) {
        std::ifstream input("../resources/tiles/associated_texture_atlas.fml");
        fml::from_stream(input, global_name_to_tileset);

        global_name_to_tileset_initialized = true;
    }

    return global_name_to_tileset;
}

// std::map<GraphicsContext*, std::shared_ptr<ResourceCache<TextureAtlas>>> TextureAtlas::atlas_caches;



// Need to inherit constructors manually.
// NOTE: This will, and are required to, copy the message.
TextureAtlas::LoadException::LoadException(const char *message): std::runtime_error(message) {}
TextureAtlas::LoadException::LoadException(const std::string &message): std::runtime_error(message) {}



std::shared_ptr<TextureAtlas> TextureAtlas::new_resource(const std::string resource_name) {
    std::shared_ptr<TextureAtlas> atlas = std::make_shared<TextureAtlas>(resource_name);

    try {
        atlas->load_names(resource_name.substr(0, resource_name.find_last_of('.')));
    }
    catch (std::exception &e) {
        LOG(WARNING) << "No names loaded for texture atlas \"" << resource_name << "\": " << e.what();
    }
    return atlas;
}


void TextureAtlas::merge(const std::vector<std::shared_ptr<TextureAtlas>> &atlases_raw) {
    std::set<std::shared_ptr<TextureAtlas>,
             std::owner_less<std::shared_ptr<TextureAtlas>>>
        atlases;
    // If we have a texture atlas which is already merged with others,
    // we need to add all of those into the list as well.
    //
    // Due to the data structure rules, we are guarenteed that sub
    // atlases have one super atlas which itself has no super atlas.
    //
    // We can only work with sub/normal atlases.
    for (auto atlas : atlases_raw) {
        if (atlas->super_atlas) {
            // It's a sub atlas.
            // Add it and all siblings.
            for (auto sub_atlas : atlas->super_atlas->sub_atlases) {
                if (std::shared_ptr<TextureAtlas> shared_atlas = sub_atlas.lock()) {
                    atlases.insert(shared_atlas);
                }
            }
        } else if (atlas->sub_atlases.size()) {
            // It's a super atlas.
            // Add only its children.
            for (auto sub_atlas : atlas->sub_atlases) {
                if (std::shared_ptr<TextureAtlas> shared_atlas = sub_atlas.lock()) {
                    atlases.insert(shared_atlas);
                }
            }
        } else {
            // It's a normal atlas (no merges yet).
            atlases.insert(atlas);
        }
    }

    for (auto atlas : atlases) {
        // Free up the old textures.
        atlas->deinit_texture();
        // Remove old super atlas(es).
        atlas->super_atlas.reset();
        atlas->reset_layout();
    }
    // Create images with allocation.
    std::shared_ptr<TextureAtlas> super_atlas = std::shared_ptr<TextureAtlas>(new TextureAtlas(atlases));

    // Update references between super and sub atlases.
    int sub_offset = 0;
    for (auto atlas : atlases) {
        atlas->index_offset = sub_offset;
        atlas->super_atlas = super_atlas;
        super_atlas->sub_atlases.push_back(std::weak_ptr<TextureAtlas>(atlas));
        sub_offset += atlas->get_texture_count();
    }
}



TextureAtlas::TextureAtlas(const std::set<std::shared_ptr<TextureAtlas>, std::owner_less<std::shared_ptr<TextureAtlas>>> &atlases):
    gl_texture(0),
    unit_w(Engine::get_tile_size()),
    unit_h(Engine::get_tile_size()),
    sub_atlases(atlases.size()),
    super_atlas(),
    names_to_indexes()
{
    int texture_count = 0;
    // Assume all tiles are the same size. They should be...
    for (auto atlas : atlases) {
        texture_count += atlas->get_texture_count();
    }

    int max_texture_size;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
    int max_columns = max_texture_size / unit_w;

    // Naive method for getting a fitting atlas size.
    if (max_columns >= texture_count) {
        unit_columns = texture_count;
        unit_rows    = 1;
    }
    else {
        unit_columns = max_columns;
        // Round up divide.
        unit_rows    = (texture_count + max_columns - 1) / max_columns;
    }
    gl_image = image = Image(unit_w * unit_columns, unit_h * unit_rows, true);

    LOG(INFO) << "Generating super atlas: textures: " << texture_count << " = (" << unit_columns << ", " << unit_rows << ") => pixels: (" << gl_image.width << ", " << gl_image.height << ")";

    textures = std::vector<std::weak_ptr<Texture>>(unit_columns * unit_rows);


    int super_i = 0;
    for (auto atlas : atlases) {
        LOG(INFO) << "Merging: " << this << " << " << atlas;
        // Cached dereference.
        Image* src = &atlas->image;
        for (int i = 0, end = atlas->get_texture_count(); i < end; ++i, ++super_i) {
            std::pair<int,int> super_units(index_to_units(super_i));
            int dst_x_offset = super_units.first  * unit_w;
            int dst_y_offset = super_units.second * unit_h;
            std::pair<int,int> units(atlas->index_to_units(i));
            int src_x_offset = units.first  * unit_w;
            int src_y_offset = units.second * unit_h;
            LOG(INFO) << "Sub-super mapping: " << i << ": (" << src_x_offset << ", " << src_y_offset << ") -> " << super_i << ": (" << dst_x_offset << ", " << dst_y_offset << ")";
            for (int y = 0; y < unit_h; ++y) {
                for (int x = 0; x < unit_w; ++x) {
                    gl_image.flipped_pixels[dst_y_offset + y][dst_x_offset + x] = (*src).flipped_pixels[src_y_offset + y][src_x_offset + x];
                }
            }
        }
    }

    init_texture();
}

TextureAtlas::TextureAtlas(const std::string image_path):
    image(image_path, true),
    gl_image(image),
    gl_texture(0),
    unit_w(Engine::get_tile_size()),
    unit_h(Engine::get_tile_size()),
    unit_columns(image.width  / unit_w),
    unit_rows   (image.height / unit_h),
    textures(unit_columns * unit_rows),
    sub_atlases(),
    super_atlas(),
    names_to_indexes()
{
    init_texture();
}


TextureAtlas::~TextureAtlas() {
    deinit_texture();
}



void TextureAtlas::init_texture() {
    int max_texture_size;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
    if (image.store_width > max_texture_size || image.store_height > max_texture_size) {
        // Turns out that the atlas is too wide or tall. Reshape it.

        int texture_count = get_texture_count();
        int max_columns = max_texture_size / unit_w;

        int old_unit_columns = unit_columns;
        int old_unit_rows = unit_rows;

        // Naive method for getting a fitting atlas size.
        if (max_columns >= texture_count) {
            unit_columns = texture_count;
            unit_rows    = 1;
        }
        else {
            unit_columns = max_columns;
            // Round up divide.
            unit_rows    = (texture_count + max_columns - 1) / max_columns;
        }
        gl_image = Image(unit_w * unit_columns, unit_h * unit_rows, true);

        LOG(INFO) << "Reshaping: " << this << ": (" << image.width << ", " << image.height << ") -> (" << gl_image.width << ", " << gl_image.height << ")";;
        LOG(INFO) << "  (Units): " << this << ": (" << old_unit_columns << ", " << old_unit_rows << ") -> (" << unit_columns << ", " << unit_rows << ")";;
        for (int i = 0, end = old_unit_columns * old_unit_rows; i < end; ++i) {
            int dst_x_offset = (i % unit_columns) * unit_w;
            int dst_y_offset = (i / unit_columns) * unit_h;

            int src_x_offset = (i % old_unit_columns) * unit_w;
            int src_y_offset = (i / old_unit_columns) * unit_h;

            LOG(INFO) << "Moving: " << i << ": (" << src_x_offset << ", " << src_y_offset << ") -> (" << dst_x_offset << ", " << dst_y_offset << ")";
            for (int y = 0; y < unit_h; ++y) {
                for (int x = 0; x < unit_w; ++x) {
                    gl_image.flipped_pixels[dst_y_offset + y][dst_x_offset + x] = image.flipped_pixels[src_y_offset + y][src_x_offset + x];
                }
            }
        }
        textures = std::vector<std::weak_ptr<Texture>>(unit_columns * unit_rows);
    }

    deinit_texture();
    glGenTextures(1, &gl_texture);

    if (gl_texture == 0) {
        LOG(ERROR) << "Unable to generate GL texture.";
        throw TextureAtlas::LoadException("Unable to generate GL texture");
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gl_texture);
    glGetError();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gl_image.store_width, gl_image.store_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, gl_image.pixels);
    if (int e = glGetError()) {
        std::stringstream hex_error_code;
        hex_error_code << std::hex << e;
        glDeleteTextures(1, &gl_texture);
        throw TextureAtlas::LoadException("Unable to load texture into GPU: " + hex_error_code.str());
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void TextureAtlas::deinit_texture() {
    if (gl_texture != 0) {
        glDeleteTextures(1, &gl_texture);
        gl_texture = 0;
    }
}



void TextureAtlas::set_tile_size(int unit_w, int unit_h) {
    this->unit_w = unit_w;
    this->unit_h = unit_h;
    reset_layout();
    init_texture();
}

void TextureAtlas::reset_layout() {
    unit_columns = image.width / unit_w;
    unit_rows    = image.height / unit_h;
    textures = std::vector<std::weak_ptr<Texture>>(unit_columns * unit_rows);
}



GLuint TextureAtlas::get_gl_texture() {
    if (super_atlas) {
        return super_atlas->get_gl_texture();
    } else {
        return gl_texture;
    }
}


GLuint TextureAtlas::offset_index(int index) {
    if (super_atlas) {
        return super_atlas->offset_index(index + index_offset);
    } else {
        return index;
    }
}


GLuint TextureAtlas::deoffset_index(int index) {
    if (super_atlas) {
        return super_atlas->deoffset_index(index - index_offset);
    } else {
        return index;
    }
}


std::pair<int,int> TextureAtlas::get_atlas_size() {
    return std::make_pair(gl_image.width, gl_image.height);
}


int TextureAtlas::get_texture_count() {
    return unit_columns * unit_rows;
}


std::pair<int,int> TextureAtlas::get_unit_size() {
    return std::make_pair(unit_w, unit_h);
}


std::pair<float,float> TextureAtlas::get_unit_size_ratio() {
    return units_to_floats(std::make_pair(1, 1));
}


int TextureAtlas::units_to_index(std::pair<int,int> units) {
    if (super_atlas) {
        return deoffset_index(units_to_index(units));
    } else {
        return units.first + unit_columns * units.second;
    }
}
std::pair<int,int> TextureAtlas::index_to_units(int index) {
    if (super_atlas) {
        return index_to_units(offset_index(index));
    } else {
        return std::make_pair(index % unit_columns,
                              index / unit_columns);
    }
}


std::pair<float,float> TextureAtlas::units_to_floats(std::pair<int,int> units) {
    if (super_atlas) {
        return super_atlas->units_to_floats(units);
    } else {
        return std::make_pair(float(units.first  * unit_w) / float(gl_image.store_width),
                              float(gl_image.height - units.second * unit_h) / float(gl_image.store_height));
    }
}


std::tuple<float,float,float,float> TextureAtlas::index_to_coords(int index) {
    if (super_atlas) {
        return super_atlas->index_to_coords(offset_index(index));
    } else {
        std::pair<int,int> units = index_to_units(index);
        return std::make_tuple<float,float,float,float>
            (
             float((units.first    ) * unit_w) / float(gl_image.store_width),
             float((units.first + 1) * unit_w) / float(gl_image.store_width),
             float(gl_image.height - (units.second + 1) * unit_h) / float(gl_image.store_height),
             float(gl_image.height - (units.second    ) * unit_h) / float(gl_image.store_height)
             );
    }
}


void TextureAtlas::load_names(const std::string filename) {
    std::ifstream file(filename + ".fml");

    if (file.fail()) {
        throw TextureAtlas::LoadException("File \"" + filename + ".fml\" could not be opened.");
    }

    fml::from_stream(file, names_to_indexes);
}

int TextureAtlas::get_name_index(const std::string name) {
    LOG(INFO) << "Name to index: " << name << ": " << names_to_indexes.at(name);
    return names_to_indexes.at(name);
}

std::pair<int, std::string> TextureAtlas::from_name(const std::string tile_name) {
    auto tileset_name(TextureAtlas::names_to_tilesets().at(tile_name));
    auto index(TextureAtlas::get_shared(tileset_name)->get_name_index(tile_name));
    return std::make_pair(index, tileset_name);
}

std::map<std::string, int> const &TextureAtlas::get_names_to_indexes() {
    return names_to_indexes;
}

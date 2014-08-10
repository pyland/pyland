// Try funky initialization in if.

#include <memory>

#include "texture_atlas.h"

#include "image.hpp"



std::map<GraphicsContext*, std::shared_ptr<ResourceCache<TextureAtlas>>> TextureAtlas::atlas_caches;



// Need to inherit constructors manually.
// NOTE: This will, and are required to, copy the message.
TextureAtlas::LoadException::LoadException(const char *message): std::runtime_error(message) {}
TextureAtlas::LoadException::LoadException(const std::string &message): std::runtime_error(message) {}



std::shared_ptr<TextureAtlas> TextureAtlas::new_shared(const std::string resource_name) {
    return std::make_shared<TextureAtlas>(resource_name + ".png");
}



TextureAtlas::TextureAtlas(const std::string image_path):
    image(image_path, true)
{
    glGenTextures(1, &gl_texture);
    
    if (gl_texture == 0) {
        LOG(ERROR) << "Unable to generate GL texture.";
        throw TextureAtlas::LoadException("Unable to generate GL texture");
    }
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gl_texture);
    glGetError();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, image.store_width, image.store_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.pixels);
    if (int e = glGetError()) {
        std::stringstream hex_error_code;
        hex_error_code << std::hex << e;
        glDeleteTextures(1, &gl_texture);
        throw TextureAtlas::LoadException("Unable to load texture into GPU: " + hex_error_code.str());
    }
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}



TextureAtlas::~TextureAtlas() {
    glDeleteTextures(1, &gl_texture);
}



GLuint TextureAtlas::get_gl_texture() {
    return gl_texture;
}


std::pair<int,int> TextureAtlas::get_atlas_size() {
    return std::make_pair<int,int>(image.width, image.height);
}


std::pair<int,int> TextureAtlas::get_unit_size() {
    return std::make_pair<int,int>(unit_w, unit_h);
}


std::pair<float,float> TextureAtlas::get_unit_size_ratio() {
    return std::make_pair<float,float>(float(unit_w) / float(image.store_width),
                                       float(unit_h) / float(image.store_height));
}


std::pair<int,int> TextureAtlas::index_to_units(int index) {
    return std::make_pair<int,int>(index % unit_columns,
                                   index / unit_rows)
}


std::pair<float,float> TextureAtlas::units_to_floats(std::pair<int,int> units) {
    return std::make_pair<float,float>(float(units.first  * unit_w) / float(image.store_width),
                                       float(units.second * unit_h) / float(image.store_height));
}


std::tuple<float,float,float,float> TextureAtlas::index_to_coords(int index) {
    std::pair<int,int> units = index_to_units(index);
    return std::make_tuple<float,float,float,float>
        (
         ((units.first     ) * unit_w) / float(image.store_width ),
         ((units.first  + 1) * unit_w) / float(image.store_width ),
         ((units.second    ) * unit_h) / float(image.store_height),
         ((units.second + 1) * unit_h) / float(image.store_height)
         );
}

#include <ext/alloc_traits.h>
#include <glog/logging.h>               // for LOG, LogMessage, etc
#include <memory>
#include <ostream>                      // for operator<<, basic_ostream, etc
#include <tuple>
#include <vector>                       // for vector

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
#include "texture.hpp"
#include "texture_atlas.hpp"



// Need to inherit constructors manually.
// NOTE: This will, and are required to, copy the message.
Texture::LoadException::LoadException(const char *message): std::runtime_error(message) {}
Texture::LoadException::LoadException(const std::string &message): std::runtime_error(message) {}



std::shared_ptr<Texture> Texture::get_shared(const std::string atlas_name, int index) {
    return Texture::get_shared(TextureAtlas::get_shared(atlas_name), index);
}

std::shared_ptr<Texture> Texture::get_shared(std::shared_ptr<TextureAtlas> atlas, int index) {
    if (index >= 0 && index < atlas->get_texture_count()) {
        // Check if the atlas has a shared texture stored.
        std::shared_ptr<Texture> texture = atlas->textures[index].lock();
        if (!texture) {
            // Generate the texture, as there wasn't a shared one.
            texture = std::make_shared<Texture>(atlas, index);
            atlas->textures[index] = std::weak_ptr<Texture>(texture);
            LOG(INFO) << "Created texture " << index << " from atlas " << atlas;
        }
        else {
            LOG(INFO) << "Reusing texture " << index << " from atlas " << atlas;
        }
        return texture;
    }
    else {
        LOG(ERROR) << "Index into texture atlas is out of range: " << atlas << " " << index;
        throw Texture::LoadException("Index into texture atlas is out of range");
    }
}



Texture::Texture(const std::string atlas_name, int index):
    Texture(TextureAtlas::get_shared(atlas_name), index) {
}

Texture::Texture(std::shared_ptr<TextureAtlas> atlas, int index):
    atlas(atlas),
    index(index)
{
    // std::tuple<float,float,float,float> bounds(atlas.index_to_coords(index));
    // x1 = std::get<0>(bounds);
    // x2 = std::get<1>(bounds);
    // y1 = std::get<2>(bounds);
    // y2 = std::get<3>(bounds);
}



GLuint Texture::get_gl_texture() {
    return atlas->get_gl_texture();
}


std::shared_ptr<TextureAtlas> Texture::get_atlas() {
    return atlas;
}


std::tuple<float,float,float,float> Texture::get_bounds() {
    return atlas->index_to_coords(index);
}

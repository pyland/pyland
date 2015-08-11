#ifndef TEXTURE_H
#define TEXTURE_H

#include <memory>
#include <stdexcept>
#include <string>
#include <tuple>

#include "open_gl.hpp"



class TextureAtlas;



///
/// A high-level texture which indexes into a lower-level texture atlas.
///
/// Textures themselves are not cacheable resources. Instead, they are
/// cached within the texture atlas object.
///
class Texture {
private:
    ///
    /// The texture atlas used by this texture.
    ///
    std::shared_ptr<TextureAtlas> atlas;

    // ///
    // /// The distance of the left edge from the left edge of the atlas.
    // ///
    // float x1;
    // ///
    // /// The distance of the bottom edge from the bottom edge of the
    // /// atlas.
    // ///
    // float y1;
    // ///
    // /// The distance of the right edge from the left edge of the atlas.
    // ///
    // float x2;
    // ///
    // /// The distance of the top edge from the bottom edge of the atlas.
    // ///
    // float y2;
    
    ///
    /// The index into the atlas.
    ///
    int index;
    
public:
    ///
    /// Represents a failure when loading the texture atlas.
    ///
    class LoadException: public std::runtime_error {
    public:
        LoadException(const char  *message);
        LoadException(const std::string &message);
    };

    ///
    /// Load a shared texture from an atlas.
    ///
    /// Load a texture using an atlas resource name and texture index.
    ///
    static std::shared_ptr<Texture> get_shared(const std::string atlas_name, int index);
    ///
    /// Load a shared texture from an atlas.
    ///
    /// Load a texture using an atlas shared pointer and texture index.
    ///
    static std::shared_ptr<Texture> get_shared(std::shared_ptr<TextureAtlas> atlas, int index);

    ///
    /// Load a texture using an altas resource name and texture index.
    ///
    Texture(const std::string atlas_name, int index);
    ///
    /// Load a texture using an atlas shared pointer and texture index.
    ///
    Texture(std::shared_ptr<TextureAtlas> atlas, int index);

    ///
    /// Get the underlying GL texture id.
    ///
    GLuint get_gl_texture();
    ///
    /// Get the texture atlas of the texture.
    ///
    std::shared_ptr<TextureAtlas> get_atlas();
    ///
    /// Get the gl texture boundaries for displaying the texture.
    ///
    /// @return The left, right, bottom, and top boundaries.
    ///
    std::tuple<float,float,float,float> get_bounds();
};



#endif

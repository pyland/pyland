// HARD-CODED:
//      Unit sizes are currently fixed at 32x32 pixels

#ifndef TEXTURE_ATLAS_H
#define TEXTURE_ATLAS_H

#include <memory>
#include <stdexcept>
#include <tuple>
#include <utility>

extern "C" {
#ifdef USE_GLES
#include <GLES2/gl2.h>
#endif
#ifdef USE_GL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif
}

#include "cacheable_resource.hpp"
#include "image.hpp"



class Texture;



///
/// A texture atlas is used to hold the texture data for one or more
/// textures.
///
/// The individual addressable textures stored within the atlas are
/// refered to as units where simple using 'texture' may confuse. These
/// units may be addressed using indexes or coordinates.
///
class TextureAtlas : public CacheableResource<TextureAtlas> {
private:
    friend class CacheableResource<TextureAtlas>;
    friend class Texture;
    ///
    /// Image used to store the pixel data in main memory.
    ///
    Image image;
    ///
    /// The GL texture id
    ///
    GLuint gl_texture;
    ///
    /// The width of a single indexable texture.
    ///
    int unit_w = 32;
    ///
    /// The height of a single indexable texture.
    ///
    int unit_h = 32;
    ///
    /// The number of columns of units in the texture.
    ///
    int unit_columns;
    ///
    /// The number of rows of units in the texture.
    ///
    int unit_rows;
    ///
    /// The shared textures which use this atlas.
    ///
    std::vector<std::weak_ptr<Texture>> textures;
    
    ///
    /// A map of texture names to texture indexes.
    ///
    std::map<std::string,int> names_to_indexes;
    
    ///
    /// Get a commonly used texture.
    ///
    /// This function is used to share textures between separate
    /// enitities and maps. On first run with given parameters it will
    /// load the texture, on other calls it will retrieve it from a
    /// cache. There is a separate cache for each GL context.
    ///
    /// @param resource_name Path of a texture image file.
    /// @return A shared pointer to the relevant Texture.
    ///
    static std::shared_ptr<TextureAtlas> new_resource(const std::string resource_name);
    
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
    /// Load a texture from a given file path.
    ///
    TextureAtlas(const std::string filename);

    ~TextureAtlas();
    
    ///
    /// Gets the underlying GL texture.
    ///
    GLuint get_gl_texture();
    ///
    /// Gets the size of the image in pixels.
    ///
    std::pair<int,int> get_atlas_size();
    ///
    /// Gets the number of textures which are stored in the image.
    ///
    int get_texture_count();
    ///
    /// Gets the size of the smallest indexable unit in pixels.
    ///
    std::pair<int,int> get_unit_size();
    ///
    /// Gets the size of the smallest indexable unit as a float
    /// compatible with the GL texture.
    ///
    std::pair<GLfloat,GLfloat> get_unit_size_ratio();
    ///
    /// Converts an index to a coordinate into the image.
    ///
    /// The coordinate is the number of units from the bottom left.
    ///
    std::pair<int,int> index_to_units(int index);
    ///
    /// Converts coordinates in units to coordinates in GL compatible
    /// floats.
    ///
    std::pair<float,float> units_to_floats(std::pair<int,int> units);
    ///
    /// Gets the floats for use as texture coordinates from an index.
    ///
    /// @return The left, right, bottom, and top boundaries.
    ///
    std::tuple<float,float,float,float> index_to_coords(int index);
    
    ///
    /// Attempt to load the name-index mappings from a file.
    ///
    void load_names(const std::string filename);
    ///
    /// Set the mapping of a name to an index.
    ///
    void set_name_index(const std::string name, int index);

    ///
    /// Get the mapping of a name to an index.
    ///
    /// @return Non-negative value of an index, or -1 if not found.
    ///
    int get_name_index(const std::string name);
};



#endif

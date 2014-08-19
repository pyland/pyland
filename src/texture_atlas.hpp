#ifndef TEXTURE_ATLAS_H
#define TEXTURE_ATLAS_H

#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

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
    /// Image used to store the GL ready pixel data in main memory.
    ///
    /// This may have a different tile arrangement to image.
    ///
    Image gl_image;
    ///
    /// The GL texture id
    ///
    GLuint gl_texture;
    ///
    /// The width of a single indexable texture.
    ///
    int unit_w;
    ///
    /// The height of a single indexable texture.
    ///
    int unit_h;
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
    /// Any atlases which map a part of this atlas.
    ///
    /// It is invalid for a sub atlas to also be a super atlas.
    ///
    std::vector<std::weak_ptr<TextureAtlas>> sub_atlases;
    ///
    /// The super atlas (if any) which this atlas maps a part of.
    ///
    /// It is invalid for a super atlas to also be a sub atlas.
    ///
    std::shared_ptr<TextureAtlas> super_atlas;
    ///
    /// If this is a sub atlas, this is the first index in the super
    /// atlas which represents part of this atlas.
    ///
    int index_offset;

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

    ///
    /// Creates a new texture atlas from a list of other atlases.
    ///
    /// Combines image data to form a new super atlas.
    ///
    TextureAtlas(const std::set<std::shared_ptr<TextureAtlas>, std::owner_less<std::shared_ptr<TextureAtlas>>> &atlases);

    ///
    /// Allocates the gl texture from the image member.
    ///
    /// If there is already an allocated texture, it is first
    /// deallocated.
    ///
    void init_texture();

    ///
    /// Deallocates the gl texture from the image member (if needed).
    ///
    void deinit_texture();

    ///
    /// Resets the layout to follow the guide of the original image.
    ///
    /// Any reshaping due to GL texture sizes is undone.
    ///
    void reset_layout();

    ///
    /// Map of all known tile names to their tileset's name,
    /// pre-generated from the job files.
    ///
    static std::map<std::string, std::string> global_name_to_tileset;

    ///
    /// When false, we still need to read values from the FML file.
    ///
    static bool global_name_to_tileset_initialized;

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
    /// Merge the resources of multiple texture atlases into one.
    ///
    /// This can be used to create tilesets with a common gl texture.
    /// After being merged, no guarentees can be made about the
    /// consistencies of mappings from indexes to bounds which were made
    /// before and after merging. The positioning of individual textures
    /// relative to each other may be changed.
    ///
    /// @param atlases A vector of atlases to alter the allocation of and
    ///                and share a common texture.
    ///
    static void merge(const std::vector<std::shared_ptr<TextureAtlas>> &atlases);

    ///
    /// Map of all known tile names to their tileset's name,
    /// pre-generated from the job files.   
    ///
    static std::map<std::string, std::string> const &names_to_tilesets();

    static std::pair<int, std::string> from_name(const std::string tile_name);

    ///
    /// Load a texture from a given file path.
    ///
    TextureAtlas(const std::string filename);

    ~TextureAtlas();

    ///
    /// Sets a non-standard tile size.
    ///
    void set_tile_size(int unit_w, int unit_h);

    ///
    /// Gets the underlying GL texture.
    ///
    GLuint get_gl_texture();
    ///
    /// Translates the index in a sub atlas to the super atlas index.
    ///
    GLuint offset_index(int index);
    ///
    /// Translates a super atlas index to the index in a sub atlas.
    ///
    GLuint deoffset_index(int index);
    ///
    /// Gets the size of the image in pixels.
    ///
    std::pair<int,int> get_atlas_size();
    ///
    /// Gets the number of textures which are stored in the image.
    ///
    /// This is the size of this atlas only, not any super atlases.
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
    /// Converts a coordinate into the image to an index.
    ///
    /// The coordinate is the number of units from the bottom left.
    /// The index is that for the sub atlas, whilst the units are for
    /// the atlas with the gl texture.
    ///
    int units_to_index(std::pair<int,int> units);
    ///
    /// Converts an index to a coordinate into the image.
    ///
    /// The coordinate is the number of units from the bottom left.
    /// The index is that for the sub atlas, whilst the units are for
    /// the atlas with the gl texture.
    ///
    std::pair<int,int> index_to_units(int index);
    ///
    /// Converts coordinates in units to coordinates in GL compatible
    /// floats.
    ///
    /// The units are for the atlas with the gl texture.
    ///
    std::pair<float,float> units_to_floats(std::pair<int,int> units);
    ///
    /// Gets the floats for use as texture coordinates from an index.
    ///
    /// @param index The index of the texture for this atlas.
    /// @return The left, right, bottom, and top boundaries.
    ///
    std::tuple<float, float, float, float> index_to_coords(int index);

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

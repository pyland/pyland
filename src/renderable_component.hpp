#ifndef RENDERABLE_COMPONENT_H
#define RENDERABLE_COMPONENT_H

#include "texture.hpp"
#include "shader.hpp"

#include <memory>
#include <string>

//Include GLM
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#ifdef USE_GLES

#include <GLES2/gl2.h>

#endif

#ifdef USE_GL

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#endif
///
/// This class holds all the referrences to the data needed to render
/// the geometry. The idea is that this data is generated and then put
/// into this object. Then, when an object is rendered, this object's
/// information is queried and used in the rendering loops.
///
/// For example, each object has a RenderableComponent and then in the
/// MapViewer render loop, each one is fetched and then used to draw the object
///
/// The aim is to separate the rendering code from the object and Map
/// classes and the logic in these classes so that we reduce code
/// bloat and needless copy-pasting.
///
/// THIS CLASS DELETES THE MEMORY IT IS GIVEN WHEN IT IS DESTRUCTED
///
///
///
class RenderableComponent {
    ///
    /// The buffer holding the vertex data
    ///
    GLfloat* vertex_data = nullptr;

    ///
    /// The size of the vertex data in bytes
    ///
    size_t vertex_data_size = 0;

    ///
    /// The number of vertices to render
    ///
    GLsizei num_vertices_render = 0;

    ///
    /// The buffer holding the texture coordinate data
    ///
    GLfloat* texture_coords_data = nullptr;

    ///
    /// The size of the texture coordinate buffer in bytes
    ///
    size_t texture_coords_data_size = 0;

    ///
    /// Texture holding abstracted image and texture management.
    ///
    std::shared_ptr<Texture> texture;

    ///
    /// The vertex buffer object identifier for the vertex buffer
    ///
    GLuint vbo_vertex_id = 0;

    ///
    /// The vertex buffer object identifier for the texture buffer
    ///
    GLuint vbo_texture_id = 0;

    ///
    /// The texture object identifier for the texture used by this component
    ///
    GLuint texture_obj_id = 0;

    ///
    /// The width of this component
    ///
    int width = 0;

    ///
    /// The height of this component
    ///
    int height = 0;

    ///
    /// The shader used to render this component. This is a pointer because more than
    /// one RenderableComponent can have the same shader
    ///
    std::shared_ptr<Shader> shader;

    ///
    /// The current projection matrix
    ///
    glm::mat4 projection_matrix = glm::mat4(1.0);

    ///
    /// the current modelview matrix
    ///
    glm::mat4 modelview_matrix  = glm::mat4(1.0);


public:
    RenderableComponent();
    ~RenderableComponent();

    ///
    /// Get the projection matrix
    /// @return the projection matrix
    ///
    glm::mat4 get_projection_matrix() { return projection_matrix; }

    ///
    /// Set the projection matrix for this component
    /// @new_projection_matrix the new matrix
    ///
    void set_projection_matrix(glm::mat4 new_projection_matrix) { projection_matrix = new_projection_matrix; }

    ///
    /// Get the modelview matrix
    /// @return the modelview matrix for this component
    ///
    glm::mat4 get_modelview_matrix() { return modelview_matrix; }

    ///
    /// Set the modelview matrix for this component
    /// @param new_modelview_matrix the new modelview matrix
    ///
    void set_modelview_matrix(glm::mat4 new_modelview_matrix) { modelview_matrix = new_modelview_matrix; }

    ///
    /// Bind the shader program to the Opengl pipeline to use it for rendering
    ///
    void bind_shader();

    ///
    /// Release the shader program from the Opengl pipeline
    ///
    void release_shader();

    ///
    /// Sets the shader to use for this component
    ///
    void set_shader(std::shared_ptr<Shader> new_shader) {shader = new_shader; }

    ///
    /// Gets the shader used by this component
    ///
    std::shared_ptr<Shader> get_shader() { return shader; }

    ///
    /// Bind the vertex buffers
    ///
    void bind_vbos();

    ///
    /// Release the vertex buffers
    ///
    void release_vbos();

    ///
    /// Get a pointer to the vertex data
    ///
    GLfloat* get_vertex_data() { return vertex_data; }

    ///
    /// Get the size of the vertex data
    ///
    size_t get_vertex_data_size() { return vertex_data_size; }

    ///
    /// Set the vertex data to use for this component.
    /// @param new_vertex_data The new data to use for ther vertices of this object
    /// @param the size of the data in bytes
    /// @param is_dynamic If true, then the data for this buffer will be changed often. If false, it is static geometry
    ///
    void set_vertex_data(GLfloat* new_vertex_data, size_t data_size,  bool is_dynamic);

    ///
    /// Get a pointer to the texture coordinate data
    ///o
    GLfloat* get_texture_coords_data() { return texture_coords_data; }

    ///
    /// Set the texture coordinate data to use for this component
    /// @param new_texture_data The new data to use for ther tehadxture coordinates of this object
    /// @param data_size The size of the data in bytes
    /// @param is_dynamic If true, then the data for this buffer will be changed often. If false, it is static
    ///
    void set_texture_coords_data(GLfloat* new_texture_data, size_t data_size, bool is_dynamic);

    ///
    /// Get the texture coords data size
    ///
    size_t get_texture_coords_data_size() { return texture_coords_data_size; }
    ///
    /// Set the texture image data and metadata of this component.
    ///
    /// @param image An Image containing the texture data and metadata.
    ///
    void set_texture(std::shared_ptr<Texture> texture);


    ///
    /// Get the texture image data and metadata of this component.
    ///
    /// @returnimage An Image containing the texture data and metadata.
    ///
    std::shared_ptr<Texture> get_texture() { return texture; }


    ///
    /// Get the width of the component
    /// @return The width of the component
    ///
    int get_width() { return width; }

    ///
    /// Get the height of the component
    /// @return The height of the component
    ///
    int get_height() { return height; }

    ///
    /// Bind the texture objects
    ///
    void bind_textures();

    ///
    /// Release the texture objects
    ///
    void release_textures();

    ///
    /// Get the number of vertices to render
    ///
    GLsizei get_num_vertices_render() { return num_vertices_render; }

    ///
    /// Set the number of vertices we need to render
    ///
    void set_num_vertices_render(GLsizei num_vertices) { num_vertices_render = num_vertices; }

    ///
    /// Update the texture buffer
    /// @param offset the byte offset into the buffer
    /// @param size the size of the data to put into the buffer in bytes
    /// @param data the data to put into the buffer
    ///
    void update_texture_buffer(GLintptr offset, size_t size, GLfloat* data);

    ///
    /// Update the vertex buffer
    /// @param offset the byte offset into the buffer
    /// @param size the size of the data to put into the buffer in bytes
    /// @param data the data to put into the buffer
    ///
    void update_vertex_buffer(GLintptr offset, size_t size, GLfloat* data);

};

#endif

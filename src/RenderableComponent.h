#ifndef RENDERABLE_COMPONENT_H
#define RENDERABLE_COMPONENT_H

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
class RenderableComponent {
  GLfloat* vertex_data;
  GLfloat* texture_data;
  GLuint vbo_vertex_id;
  GLuint vbo_texture_id;

  GLuint texure_obj_id;
  int width;
  int height;

  ///
  /// The current projection matrix
  ///
  glm::mat4 projection_matrix;

  ///
  /// the current modelview matrix
  /// 
  glm::mat4 modelview_matrix;


public:
  RenderableComponent();
  ~RenderableComponent();
  ///
  /// Get a pointer to the vertex data
  ///
  GLfloat* get_vertex_data() { return vertex_data; }

  ///
  /// Set the vertex data to use for this component
  /// @param new_vertex_data The new data to use for ther vertices of this object
  /// @param the size of the data in bytes
  /// @param is_dynamic If true, then the data for this buffer will be changed often. If false, it is static geometry
  ///
  void set_vertex_data(GLfloat* new_vertex_data, int data_size,  bool is_dynamic);

  ///
  /// Get a pointer to the texture data
  ///
  GLfloat* get_texture_data() { return texture_data; }

  /// 
  /// Set the texture data to use for this component
  /// @param new_texture_data The new data to use for ther texture coordinates of this object
  /// @param data_size The size of the data in bytes
  /// @param is_dynamic If true, then the data for this buffer will be changed often. If false, it is static 
  ///
  void set_texture_data(GLfloat* new_texture_data, int data_size, bool is_dynamic);

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
};

#endif

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



class RenderableComponent {
  GLfloat* vertex_data;
  GLfloat* texture_data;
  GLuint vbo_ids[];
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
  
  ///
  /// Get a pointer to the vertex data
  ///
  GLfloat* get_vertex_data() { return vertex_data; }

  ///
  /// Set the vertex data to use for this component
  ///
  void set_vertex_data(GLfloat* new_vertex_data) { vertex_data = new_vertex_data; }

  ///
  /// Get a pointer to the texture data
  ///
  GLfloat* get_texture_data() { return texture_data; }

  /// 
  /// Set the texture data to use for this component
  void set_texture_data(GLfloat* new_texture_data) { texture_data  = new_texture_data; }

  ///
  /// Get the width of the component
  /// 
  int get_width() { return width; }

  ///
  /// Get the height of the component
  ///
  int get_height() { return height; } 

  
};

#endif

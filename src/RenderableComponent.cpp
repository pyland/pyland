#include "RenderableComponent.h"

#define VERTEX_POS_INDX 0
#define  VERTEX_TEXCOORD0_INDX 1

RenderableComponent::RenderableComponent() {
  //Generate the vertex buffers
  glGenBuffers(1, &vbo_vertex_id);
  glGenBuffers(1, &vbo_texture_id);
}

RenderableComponent::~RenderableComponent() {
  //Delete the vertex buffers
  glDeleteBuffers(1, &vbo_vertex_id);
  glDeleteBuffers(1, &vbo_texture_id);
}

void RenderableComponent::set_vertex_data(GLfloat* new_vertex_data, int data_size, bool is_dynamic) { 
  vertex_data = new_vertex_data;

  //Set up buffer usage
  GLenum usage = GL_STATIC_DRAW;
  if(is_dynamic)
    usage = GL_DYNAMIC_DRAW;
  
  //Pass in data to the buffer buffer
  glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_id);
  glBufferData(GL_ARRAY_BUFFER, data_size, vertex_data, GL_STATIC_DRAW);

  //Set the attributes
  glEnableVertexAttribArray(VERTEX_POS_INDX);

  glVertexAttribPointer(VERTEX_POS_INDX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
}

void RenderableComponent::set_texture_data(GLfloat* new_texture_data, int data_size, bool is_dynamic) { 
  texture_data = new_texture_data;


  //Set up buffer usage
  GLenum usage = GL_STATIC_DRAW;
  if(is_dynamic)
    usage = GL_DYNAMIC_DRAW;
  
  //Pass in data to the buffer buffer
  glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_id);
  glBufferData(GL_ARRAY_BUFFER, data_size, texture_data, GL_STATIC_DRAW);

  //Set the attributes
  glEnableVertexAttribArray(VERTEX_TEXCOORD0_INDX);

  glVertexAttribPointer(VERTEX_TEXCOORD0_INDX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
}

#include <glog/logging.h>
#include "image.hpp"
#include "renderable_component.hpp"

#define VERTEX_POS_INDX 0
#define  VERTEX_TEXCOORD0_INDX 1
#include <iostream>
RenderableComponent::RenderableComponent() {

    //Generate the vertex buffers
    glGenBuffers(1, &vbo_vertex_id);
    glGenBuffers(1, &vbo_texture_id);
    LOG(INFO) << "RenderableComponent::RenderableComponent: Buffers "<< vbo_vertex_id;
    LOG(INFO) << "RenderableComponent::RenderableComponent: Buffers " << vbo_texture_id;
}

RenderableComponent::~RenderableComponent() {
    //Delete the vertex buffers
    glDeleteBuffers(1, &vbo_vertex_id);
    glDeleteBuffers(1, &vbo_texture_id);
    

    //TODO delete textures - need to see if texture manager does this
    delete []vertex_data;
    delete []texture_coords_data;

    //TODO: Texture manager will do this
    //    delete texture_image;
}
void RenderableComponent::set_vertex_data(GLfloat* new_vertex_data, size_t data_size, bool is_dynamic) {
    delete []vertex_data;
    vertex_data = new_vertex_data;
    vertex_data_size = data_size;

    //Get current shader
    GLint id;
    glGetIntegerv(GL_CURRENT_PROGRAM, &id);

    //Bind our shader
    bind_shader();

    //Set up buffer usage
    GLenum usage = GL_STATIC_DRAW;
    if(is_dynamic)
        usage = GL_DYNAMIC_DRAW;
  
    //Pass in data to the buffer buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_id);
    glBufferData(GL_ARRAY_BUFFER, vertex_data_size, vertex_data, usage);

    //Restore previous program
    glUseProgram(id);
}

void RenderableComponent::set_texture_image(Image* image) { 
    //TODO: Get texture manager to prevent memory leak
    texture_image = image;

    //Get current shader
    GLint id;
    glGetIntegerv(GL_CURRENT_PROGRAM, &id);

    //Bind our shader
    bind_shader();

    glGenTextures(1, &texture_obj_id);
    glBindTexture(GL_TEXTURE_2D, texture_obj_id);
  
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_image->store_width, texture_image->store_height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, (void*)texture_image->pixels);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST);

    //Restore shader
    glUseProgram(id);
}

void RenderableComponent::set_texture_coords_data(GLfloat* new_texture_data, size_t data_size, bool is_dynamic) { 
    delete []texture_coords_data;
    texture_coords_data = new_texture_data;
    texture_coords_data_size = data_size;
    //Get current shader
    GLint id;
    glGetIntegerv(GL_CURRENT_PROGRAM, &id);

    //Bind our shader
    bind_shader();

    //Set up buffer usage
    GLenum usage = GL_STATIC_DRAW;
    if(is_dynamic)
        usage = GL_DYNAMIC_DRAW;
  
    //Pass in data to the buffer buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_id);
    glBufferData(GL_ARRAY_BUFFER, texture_coords_data_size, texture_coords_data, usage);

    //Release shader
    glUseProgram(id);
}

void RenderableComponent::bind_vbos() {
  
    //Bind the vertex data buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_id);
    glVertexAttribPointer(0 /*VERTEX_POS_INDX*/, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0 /* VERTEX_POS_INDX */);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_id);
    glVertexAttribPointer(1 /* VERTEX_TEXCOORD0_INDX */, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray( 1 /*VERTEX_TEXCOORD0_INDX*/);
    //    glBindAttribLocation(shader->get_program(), glGetAttribLocation(shader->get_program(), "a_position") /*VERTEX_POS_INDX*/, "a_position");

    //    glBindAttribLocation(shader->get_program(), glGetAttribLocation(shader->get_program(), "a_texCoord")                         /*VERTEX_TEXCOORD0_INDX*/, "a_texCoord");


    //set sampler texture to unit 0
    glUniform1i(glGetUniformLocation(shader->get_program(), "s_texture"), 0);
}
void RenderableComponent::bind_textures() {
    glActiveTexture(GL_TEXTURE0);

    //Bind tiles texture
    glBindTexture(GL_TEXTURE_2D,texture_obj_id);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_image->store_width, texture_image->store_height, 0,
    //              GL_RGBA, GL_UNSIGNED_BYTE, (void*)texture_image->pixels);
    // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST);
    // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST);

}

void RenderableComponent::release_textures() {
    glBindTexture(GL_TEXTURE_2D, 0);
}
void RenderableComponent::release_vbos() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void RenderableComponent::bind_shader() {
    if(shader == nullptr)
        return;
    glUseProgram(shader->get_program());
}

void RenderableComponent::release_shader() {
    glUseProgram(0);
}

void RenderableComponent::update_vertex_buffer(int offset, size_t size, GLfloat* data) {
    //Get current shader
    GLint id;
    glGetIntegerv(GL_CURRENT_PROGRAM, &id);

    //Bind our shader
    bind_shader();

    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_id);

    //Update the buffer
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);

    //Release shader
    glUseProgram(id);
}

void RenderableComponent::update_texture_buffer(int offset, size_t size, GLfloat* data) {
    //Get current shader
    GLint id;
    glGetIntegerv(GL_CURRENT_PROGRAM, &id);

    //Bind our shader
    bind_shader();

    glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_id);

    //Update the buffer
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
    std::cout << " BUF " << vbo_texture_id << std::endl;
    //Release shader
    glUseProgram(id);
}


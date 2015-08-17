#include <memory>
#include <ostream>

#include <glog/logging.h>

#include "shader.hpp"
#include "texture_atlas.hpp"
#include "renderable_component.hpp"

#define VERTEX_POS_INDX 0
#define VERTEX_TEXCOORD0_INDX 1

RenderableComponent::RenderableComponent() {

    //Generate the vertex buffers
    glGenBuffers(1, &vbo_vertex_id);
    glGenBuffers(1, &vbo_texture_id);
    //LOG(INFO) << "RenderableComponent::RenderableComponent: Buffers "<< vbo_vertex_id;
    //LOG(INFO) << "RenderableComponent::RenderableComponent: Buffers " << vbo_texture_id;
}

RenderableComponent::~RenderableComponent() {
    //Delete the vertex buffers
    glDeleteBuffers(1, &vbo_vertex_id);
    glDeleteBuffers(1, &vbo_texture_id);

    delete[] vertex_data;
    delete[] texture_coords_data;
}
void RenderableComponent::set_vertex_data(GLfloat* new_vertex_data, size_t data_size, bool is_dynamic) {
    delete[] vertex_data;
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

void RenderableComponent::set_texture(std::shared_ptr<TextureAtlas> texture_atlas) {
    this->texture_atlas = texture_atlas;
}

void RenderableComponent::set_texture_coords_data(GLfloat* new_texture_data, size_t data_size, bool is_dynamic) {
    delete[] texture_coords_data;
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
    glBindTexture(GL_TEXTURE_2D, texture_atlas->get_gl_texture());
}

void RenderableComponent::release_textures() {
    glBindTexture(GL_TEXTURE_2D, 0);
}
void RenderableComponent::release_vbos() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void RenderableComponent::bind_shader() {
    if(!shader)
        return;
    glUseProgram(shader->get_program());
}

void RenderableComponent::release_shader() {
    glUseProgram(0);
}

void RenderableComponent::update_vertex_buffer(GLintptr offset, size_t size, GLfloat* data) {
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

void RenderableComponent::update_texture_buffer(GLintptr offset, size_t size, GLfloat* data) {
    //Get current shader
    GLint id;
    glGetIntegerv(GL_CURRENT_PROGRAM, &id);

    //Bind our shader
    bind_shader();

    glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_id);

    //Update the buffer
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
    LOG(INFO) << " BUF " << vbo_texture_id;
    //Release shader
    glUseProgram(id);
}


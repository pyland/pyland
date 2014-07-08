#include "Character.h"
#include "RenderableComponent.h"
#include <iostream>


#ifdef USE_GLES

#include <GLES2/gl2.h>

#endif

#ifdef USE_GL

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#endif

#define TILESET_ELEMENT_SIZE 16
#define IMAGE2_SIZE_WIDTH 198
#define IMAGE2_SIZE_HEIGHT 128
#define GLOBAL_SCALE 2

Character::Character() {

}

Character::~Character() {
  delete []sprite_tex_data;
}


void Character::generate_tex_data() {
  RenderableComponent* render_component = get_renderable_component();
  if(render_component == NULL) {
    std::cerr << "ERROR in Character::generate_tex_data, render_component is NULL" << std::endl;
    return;
  }
  

  //holds the texture data
  //need 12 float for the 2D texture coordinates
  int num_floats = 12;
  sprite_tex_data = new GLfloat[sizeof(GLfloat)*num_floats]; 
  if(sprite_tex_data == NULL) {
    std::cerr << "ERROR in Character::generate_tex_data, cannot allocate memory" << std::endl;
    return;
  }

  GLfloat offset_x = GLfloat(1.0 / (IMAGE2_SIZE_WIDTH  / TILESET_ELEMENT_SIZE));
  GLfloat offset_y = GLfloat(1.0 / (IMAGE2_SIZE_HEIGHT / TILESET_ELEMENT_SIZE));

  //bottom left
  sprite_tex_data[0] = offset_x;
  sprite_tex_data[1] = offset_y;

  //top left
  sprite_tex_data[2] = offset_x;
  sprite_tex_data[3] = 0.0f; 

  //bottom right
  sprite_tex_data[4] = offset_x*2.0f;
  sprite_tex_data[5] = offset_y;

  //top left
  sprite_tex_data[6] = offset_x;
  sprite_tex_data[7] = 0.0f;

  //top right
  sprite_tex_data[8] = offset_x*2.0f;
  sprite_tex_data[9] = 0.0f;

  //bottom right
  sprite_tex_data[10] = offset_x*2.0f;
  sprite_tex_data[11] = offset_y;

  render_component->set_texture_coords_data(sprite_tex_data, num_floats, true);
} 

void Character::generate_vertex_data() {
  RenderableComponent* render_component = get_renderable_component();
  if(render_component == NULL) {
    std::cerr << "ERROR in Character::generate_tex_data, render_component is NULL" << std::endl;
    return;
  }

  //holds the character vertex data
  //need 18 floats for each coordinate as these hold 3D coordinates
  int num_floats = 18;
  sprite_data  = new GLfloat[sizeof(GLfloat)*num_floats]; 
  if(sprite_data == NULL) {
    std::cerr << "ERROR in Characater::genrate_vertex_data, cannot allocate memory" << std::endl;
    return;
  }
  

  float scale = TILESET_ELEMENT_SIZE * GLOBAL_SCALE;
  //bottom left 
  sprite_data[0] = 0;
  sprite_data[1] = 0;
  sprite_data[2] = 0;
       
  //top left
  sprite_data[3] = 0;
  sprite_data[4] = (1) * scale;
  sprite_data[5] = 0;

  //bottom right
  sprite_data[6] = (1) * scale;
  sprite_data[7] = 0;
  sprite_data[8] = 0;

  //top left
  sprite_data[9] = 0;
  sprite_data[10] = 1 * scale;
  sprite_data[11] = 0;

  //top right
  sprite_data[12] = 1 * scale;
  sprite_data[13] = 1 * scale;
  sprite_data[14] = 0;

  //bottom right
  sprite_data[15] = 1 * scale;
  sprite_data[16] = 0;
  sprite_data[17] = 0;

  render_component->set_vertex_data(sprite_data, num_floats, true);
  render_component->set_num_vertices_render(num_floats/3);//GL_TRIANGLES being used
}

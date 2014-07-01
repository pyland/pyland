/*
Copyright (c) 2012, Broadcom Europe Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without
omodification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
* Neither the name of the copyright holder nor the
names of its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// A rotating cube rendered with OpenGL|ES. Three images used as textures on the cube faces.
    
#include <cstdio>
#include <cstdlib>

#include <cmath>
#include <cassert>
#include <unistd.h>
#include <ctime>
#include <sys/time.h>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>

 //Include GLM
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "game_window.hpp"
#include "python_embed/interpreter.h"

#ifdef USE_GLES

#include <GLES2/gl2.h>

#endif

#ifdef USE_GL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif


#include "main.h"

#define PATH "./"

#define IMAGE_SIZE_WIDTH 128
#define IMAGE_SIZE_HEIGHT 128
#ifndef M_PI
#define M_PI 3.141592654
#endif
using namespace std;	


static  void init_ogl();
static void init_model_proj(GameWindow* window);
static void redraw_scene(GameWindow* window, float dt);
static void update(float dt);
static void init_textures();
static void load_tex_images();
static void exit_func(void);

static void draw_sprites( float dt);
void generate_sprite_coords();
void generate_sprite_tex_data();
/**
 * Move the object, identified by the id, in the specified direction.
 * 0 = move up, 1 = move right, 2 = move down, 3 = move left
 */
void move_object(const int id, const int direction);

static volatile int shutdown;

#define VERTEX_POS_INDX 0
#define VERTEX_TEXCOORD0_INDX 1
const int num_vbo_ids = 4;
static void draw_map(int image_height, int image_width, float dt);
GLuint vboIds[num_vbo_ids];
GLuint program_obj =0;
GLuint texture_id = 0;
const int map_height = 16;
const int map_width = 16;
float map_scroll_speed = 32.0f; //1 tile a second
float map_bottom_x = 0.0;
float map_bottom_y = 0.0; 
const int map_display_width = 8;
const int map_display_height = 8;

GLfloat* sprite_data;
GLfloat* sprite_tex_data;

char* tex_buf1;
glm::mat4 projection_matrix;
const int num_objects = 2;



GLfloat * tileSetTexCoords;
GLfloat * mapData;
GLfloat * mapTexCoords;
// Spatial coordinates for the cube
static const GLbyte quadx[4*3] = {
   /* FRONT */
   -10, -10, 0,
   10, -10,  0,
   -10,  10, 0,
   10,  10,  0,
};

/** Texture coordinates for the quad. */
static const GLfloat texCoords[ 4 * 2] = {
   0.f,  0.f,
   0.f,  1.f,
   1.f,  0.f,
   1.f,  1.f,
};

/** Holds the overall map data */
static const int worldData[] = {
  10, 10, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 0, 10, 2, 0, 0, 4,0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 0, 10, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 2, 0, 0, 3,0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 2, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

};



struct Object {
  float x;
  float y;
} objects[num_objects];

void move_object(const int id, const int direction) {
  if(id > num_objects || id < 0) {
    cerr << "ERROR: mov_object: object id exceeds number of objects. Object id: " << id << endl;
    return;
  } 
  if(direction < 0 || direction > 3) {
    cerr << "ERROR: mov_object: direction is not valid " << direction << endl;
    return;
  }
 
  switch(direction) {
    //MOVE UP
  case 0:
    objects[id].y++;
    break;
    //MOVE RIGHT
  case 1:
    objects[id].x++;
   break;
    //MOVE DOWN
  case 2:
    objects[id].y--;
    break;
    //MOVE LEFT
  case 3:
    objects[id].x--;
    break;
  }

  //Wrap
  if(objects[id].x <0) objects[id].x = 600.0f;
  if(objects[id].y < 0) objects[id].y = 400.0f;
  if(objects[id].y > 400.0f) objects[id].y = 0.0f;
  if(objects[id].x > 600.0f) objects[id].x = 0.0f;
}

static void init_buffers() {
  glUseProgram(program_obj);

  //vboIds[0] = map geometric data
  //vboIds[1] = tileset
  //vboIds[2] = sprite geometric data
  //vboIds[3] = texture coords for sprite
  glGenBuffers(num_vbo_ids, vboIds);

  glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
  glBufferData(GL_ARRAY_BUFFER, map_height*map_width*sizeof(GLfloat)*18, mapData, GL_STATIC_DRAW);
  

  glBindBuffer(GL_ARRAY_BUFFER, vboIds[1]);
  glBufferData(GL_ARRAY_BUFFER, map_height*map_width*sizeof(GLfloat)*12, mapTexCoords, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vboIds[2]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*18, sprite_data, GL_STATIC_DRAW);

  //changing texture coords
  glBindBuffer(GL_ARRAY_BUFFER, vboIds[3]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*12, sprite_tex_data, GL_DYNAMIC_DRAW);

  //MAPS
  glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
  glEnableVertexAttribArray(VERTEX_POS_INDX);

  glBindBuffer(GL_ARRAY_BUFFER, vboIds[1]);
  glEnableVertexAttribArray(VERTEX_TEXCOORD0_INDX);


  //SPRITES
  glBindBuffer(GL_ARRAY_BUFFER, vboIds[2]);
  glEnableVertexAttribArray(VERTEX_POS_INDX);

  glBindBuffer(GL_ARRAY_BUFFER, vboIds[3]);
  glEnableVertexAttribArray(VERTEX_TEXCOORD0_INDX);



  glVertexAttribPointer(VERTEX_POS_INDX, 3, GL_FLOAT, GL_FALSE,0, 0);
  glVertexAttribPointer(VERTEX_TEXCOORD0_INDX, 2, GL_FLOAT, GL_FALSE, 0, 0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,texture_id);
}

static void animate(float dt) {

  //animate map
  float map_display_right_x = map_bottom_x + map_display_width;
  float map_display_top_y = map_bottom_y + map_display_height;

  //scroll the map towards the top right
  map_bottom_x += map_scroll_speed*dt;
  map_bottom_y += map_scroll_speed*dt;

  //perform wrapping
  if(map_display_right_x > map_width) {
    map_bottom_x = 0.0f;//wrap round
  }

  if(map_display_top_y > map_height) {
    map_bottom_y = 0.0f; //wrap round
  }


  //animate characters
  static float time_to_next = 0.0f;
  static int sequence[] = {1, 2, 3, 5, 7, 9, 11};
  static int i = 0;

  if(i == sizeof(sequence)/sizeof(int))
    i = 0;

  time_to_next += dt;
  if(time_to_next > (1.0f/60.0f)) {
    time_to_next = 0.0f;
    
    int currTile = sequence[i++];
    GLfloat *tileSetPtr = &tileSetTexCoords[currTile*8];
    //bottom left
    sprite_tex_data[0] = tileSetPtr[0];
    sprite_tex_data[1] = tileSetPtr[1];

    //top left
    sprite_tex_data[2] = tileSetPtr[2];
    sprite_tex_data[3] = tileSetPtr[3];

    //bottom right
    sprite_tex_data[4] = tileSetPtr[4];
    sprite_tex_data[5] = tileSetPtr[5];

    //top left
    sprite_tex_data[6] = tileSetPtr[2];
    sprite_tex_data[7] = tileSetPtr[3];

    //top right
    sprite_tex_data[8] = tileSetPtr[6];
    sprite_tex_data[9] = tileSetPtr[7];
	
    //bottom right
    sprite_tex_data[10] = tileSetPtr[4];
    sprite_tex_data[11] = tileSetPtr[5];
    //changing texture coords
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*12, sprite_tex_data, GL_DYNAMIC_DRAW);

  }

}
static void draw_map(int map_width, int map_height, float dt)
 {

   //LEAVE THIS HERE!
   glDisable(GL_CULL_FACE);
   glm::mat4 model = glm::mat4(1.0f);
   glm::mat4 translated = glm::translate(model, glm::vec3(map_bottom_x, map_bottom_y, 0.0f));

   glUseProgram(program_obj);

   glUniformMatrix4fv(glGetUniformLocation(program_obj, "mat_projection"), 1, GL_FALSE,glm::value_ptr(projection_matrix));
   glUniformMatrix4fv(glGetUniformLocation(program_obj, "mat_modelview"), 1, GL_FALSE, glm::value_ptr(translated));


   glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
   glVertexAttribPointer(VERTEX_POS_INDX, 3, GL_FLOAT, GL_FALSE, 0, 0);
   glEnableVertexAttribArray(VERTEX_POS_INDX);


   glBindBuffer(GL_ARRAY_BUFFER, vboIds[1]);
   glVertexAttribPointer(VERTEX_TEXCOORD0_INDX, 2, GL_FLOAT, GL_FALSE, 0, 0);
   glEnableVertexAttribArray(VERTEX_TEXCOORD0_INDX);

   glBindAttribLocation(program_obj, VERTEX_POS_INDX, "a_position");


   glBindAttribLocation(program_obj, VERTEX_TEXCOORD0_INDX, "a_texCoord");

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D,texture_id);

   //set sampler texture to unit 0
   glUniform1i(glGetUniformLocation(program_obj, "s_texture"), 0);


   glDrawArrays(GL_TRIANGLES, 0, 6*map_width*map_height);
   glUseProgram(0);
 }

static void draw_sprites( float dt)
 {
   glm::mat4 model = glm::mat4(1.0f);

   glUseProgram(program_obj);

   glUniformMatrix4fv(glGetUniformLocation(program_obj, "mat_projection"), 1, GL_FALSE,glm::value_ptr(projection_matrix));



   glBindBuffer(GL_ARRAY_BUFFER, vboIds[2]);
   glVertexAttribPointer(VERTEX_POS_INDX, 3, GL_FLOAT, GL_FALSE, 0, 0);
   glEnableVertexAttribArray(VERTEX_POS_INDX);


   glBindBuffer(GL_ARRAY_BUFFER, vboIds[3]);
   glVertexAttribPointer(VERTEX_TEXCOORD0_INDX, 2, GL_FLOAT, GL_FALSE, 0, 0);
   glEnableVertexAttribArray(VERTEX_TEXCOORD0_INDX);

   glBindAttribLocation(program_obj, VERTEX_POS_INDX, "a_position");


   glBindAttribLocation(program_obj, VERTEX_TEXCOORD0_INDX, "a_texCoord");

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D,texture_id);

   //set sampler texture to unit 0
   glUniform1i(glGetUniformLocation(program_obj, "s_texture"), 0);

   for(int i = 0; i< num_objects; i++) {
        glm::mat4 translated = glm::translate(model, glm::vec3(objects[i].x, objects[i].y, 0.0f));
   glUniformMatrix4fv(glGetUniformLocation(program_obj, "mat_modelview"), 1, GL_FALSE, glm::value_ptr(translated));

   
     glDrawArrays(GL_TRIANGLES, 0, 6);
   }
   glUseProgram(0);
 }


 /***********************************************************
  * Initialise the opengl specifics for the game
  *
  ***********************************************************/
 static void init_ogl()
 {
   // Set background color and clear buffers
   glClearColor(0.15f, 0.25f, 0.35f, 1.0f);
   
   //Leave this here!!!
   // Disable back face culling.
   glDisable(GL_CULL_FACE);

 }

 /***********************************************************
  * Name: init_model_proj

  * Description: Sets the OpenGL|ES model to default values
  *
  * Returns: void
  *
  ***********************************************************/

 static void init_model_proj(GameWindow *window)
 {
   std::pair<int, int> size = window->get_size();
  glViewport(0, 0,  size.first, size.second);
 projection_matrix = glm::ortho(0.0f, (float)(size.first), 0.0f, (float)(size.second), -1.0f, 1.0f);
  //  glViewport(0, 0,  640, 480);
  //  projection_matrix = glm::ortho(0.0f, 640.0f, 0.0f, 480.0f, -1.0f, 1.0f);
 }


 /***********************************************************
  * Name: update_model
  *

  * Description: Updates model projection to current position/rotation
  *
  * Returns: void
  *
  ***********************************************************/
 static void update(float dt)
 {

   animate(dt);
   
 }

 /***********************************************************
  * Name: redraw_scene
  *

  * Description:   Draws the model and calls eglSwapBuffers
  *                to render to screen
  *
  * Returns: void
  *
  ***********************************************************/
 static void redraw_scene(GameWindow *window, float dt)
 {
   glClear( GL_COLOR_BUFFER_BIT );

   draw_map(16, 16, dt);
   draw_sprites( dt);
   window->swap_buffers();
 }


 /***********************************************************
  * Name: generate_tilset_coords
  *
  ***********************************************************/
 static void generate_tileset_coords(int image_height, int image_width)
 {

 #ifdef DEBUG
   printf("GENERATING TILESET TEXTURE COORDS...");
 #endif
   //check the tilset image height and widths are multiples of the tiles
   //  assert(image_height % TILESET_ELEMENT_SIZE != 0 || image_width % TILESET_ELEMENT_SIZE != 0);


   int numTilesX = image_width/ TILESET_ELEMENT_SIZE;
   int numTilesY = image_height / TILESET_ELEMENT_SIZE;

   //Each tile needs 8 floats to describe its position in the image
   tileSetTexCoords = new GLfloat[sizeof(GLfloat)* numTilesX * numTilesY * 4 * 2];
   assert(tileSetTexCoords != 0);
   //TODO free this memory

     double tileSetXOffset = 0.0;
  double tileSetYOffset = 0.0;
  double tileSetXInc = 1.0 / (double)numTilesX;
  double tileSetYInc = 1.0 / (double)numTilesY;
  //TODo: DIV ZEro HERRE 

  //TODO: REMEMBER TILESET COORDINATES ARE INVERSE OF IMAGE FILE ONES
  //generate the coordinates for each tile
  for(int x = 0; x < numTilesX; x++)
    {
      for(int y = 0; y< numTilesY; y++)
	{
	  //bottom left
	  tileSetTexCoords[x* numTilesY*4*2+y*(4*2)] = tileSetXOffset;
	  tileSetTexCoords[x* numTilesY*4*2+y*4*2 +1] =tileSetYOffset;

	  //top left
	  tileSetTexCoords[x* numTilesY*4*2+y*4*2+ 2] =tileSetXOffset;
	  tileSetTexCoords[x* numTilesY*4*2+y*4*2+3] = tileSetYOffset + tileSetYInc;

	  //bottom right
	  tileSetTexCoords[x* numTilesY*4*2+y*4*2+4] = tileSetXOffset + tileSetXInc;
	  tileSetTexCoords[x* numTilesY*4*2+y*4*2+5] = tileSetYOffset;

	  //top right
	  tileSetTexCoords[x* numTilesY*4*2+y*4*2+6] = tileSetXOffset + tileSetXInc;
	  tileSetTexCoords[x* numTilesY*4*2+y*4*2+7] = tileSetYOffset + tileSetYInc;

	  tileSetYOffset += tileSetYInc;
	}
      tileSetXOffset += tileSetXInc;
      tileSetYOffset = 0.0;
    }
}


/***********************************************************
 * Name: generate_map_texcoords
 * Description: This code generates the texture coordinate
 *     that we use for the map
 *
 ***********************************************************/
static void generate_map_texcoords(int map_width, int map_height)
{

#ifdef DEBUG
  printf("GENERATING MAP TEXTURE DATA...");
#endif
  //holds the map data
  //need 12 float for the 2D texture coordinates
  int num_floats = 12;
  mapTexCoords = new GLfloat[sizeof(GLfloat)*map_height*map_width*num_floats]; 
  assert(mapTexCoords != 0);

  int x, y;

  //generate the map data
  // get the tile set coordinates for the particular tile
  for(x = 0; x < map_width; x++)
    {
      for(y = 0; y < map_height; y++)
	{
	  int currTile = worldData[x*map_height +y];
	  GLfloat *tileSetPtr = &tileSetTexCoords[currTile*8];
	  //bottom left
	  mapTexCoords[x*map_height*num_floats + y*num_floats+0] = tileSetPtr[0];
	  mapTexCoords[x*map_height*num_floats + y*num_floats+1] = tileSetPtr[1];

	  //top left
	  mapTexCoords[x*map_height*num_floats + y*num_floats+2] = tileSetPtr[2];
	  mapTexCoords[x*map_height*num_floats + y*num_floats+3] = tileSetPtr[3];

	  //bottom right
	  mapTexCoords[x*map_height*num_floats + y*num_floats+4] = tileSetPtr[4];
	  mapTexCoords[x*map_height*num_floats + y*num_floats+5] = tileSetPtr[5];

	  //top left
	  mapTexCoords[x*map_height*num_floats + y*num_floats+6] = tileSetPtr[2];
	  mapTexCoords[x*map_height*num_floats + y*num_floats+7] = tileSetPtr[3];

	  //top right
	  mapTexCoords[x*map_height*num_floats + y*num_floats+8] = tileSetPtr[6];
	  mapTexCoords[x*map_height*num_floats + y*num_floats+9] = tileSetPtr[7];
	
	  //bottom right
	  mapTexCoords[x*map_height*num_floats + y*num_floats+10] = tileSetPtr[4];
	  mapTexCoords[x*map_height*num_floats + y*num_floats+11] = tileSetPtr[5];

 	}
    }
}

/***********************************************************
 * Name: generate_map_coords
 * Description: This code generates the vertex data for the map
 *
 ***********************************************************/
static void generate_map_coords(int map_width, int map_height)
{
#ifdef DEBUG
  printf("GENERATING MAP DATA...");
#endif
  //holds the map data
   //need 18 floats for each coordinate as these hold 3D coordinates
  int num_floats = 18;
  mapData = new GLfloat[sizeof(GLfloat)*map_height*map_width*num_floats]; 
  assert(mapData != 0);
  float scale = 16.0f;
  //generate the map data
  /**
   * Vertex winding order:
   * 1, 3   4
   *  * --- * 
   *  |     |
   *  |     | 
   *  * --- *
   * 0       2,5
   */
  int x, y;
  for(x = 0; x < map_width; x++)
    {
      for(y = 0; y < map_height; y++)
	{
	  //generate one tile's worth of data

	  
	  //bottom left
	  mapData[x*map_height*num_floats + y*num_floats+0] = x * scale;
	  mapData[x*map_height*num_floats + y*num_floats+1] = y * scale;
	  mapData[x*map_height*num_floats + y*num_floats+2] = 0;
	   
	  //top left
	  mapData[x*map_height*num_floats + y*num_floats+3] = x * scale;
	  mapData[x*map_height*num_floats + y*num_floats+4] = (y+1) * scale;
	  mapData[x*map_height*num_floats + y*num_floats+5] = 0;

	  //bottom right
	  mapData[x*map_height*num_floats + y*num_floats+6] = (x+1) * scale;
	  mapData[x*map_height*num_floats + y*num_floats+7] = y * scale;
	  mapData[x*map_height*num_floats + y*num_floats+8] = 0;
	
	  //top left
	  mapData[x*map_height*num_floats + y*num_floats+9] = x * scale;
	  mapData[x*map_height*num_floats + y*num_floats+10] = (y+1) * scale;
	  mapData[x*map_height*num_floats + y*num_floats+11] = 0;
  
   	  //top right
	  mapData[x*map_height*num_floats + y*num_floats+12] = (x+1) * scale;
	  mapData[x*map_height*num_floats + y*num_floats+13] = (y+1) * scale;
	  mapData[x*map_height*num_floats + y*num_floats+14] = 0;

	  //bottom right
	  mapData[x*map_height*num_floats + y*num_floats+15] = (x+1) * scale;
	  mapData[x*map_height*num_floats + y*num_floats+16] = y * scale;
	  mapData[x*map_height*num_floats + y*num_floats+17] = 0;
	}
    }
#ifdef DEBUG
  printf("DONE.");
#endif

}

GLuint load_shader(GLenum type, const std::string src) {
  GLuint shader;
  GLint compiled = 0;
  
  //Create the shader object 
  shader = glCreateShader(type);

  if(shader == 0) 
    return 0; //couldn't create the shader

  //Load shader source code
  const char* source = src.c_str();
  glShaderSource(shader, 1, &source, NULL);

  //Compile the shader
  glCompileShader(shader);

  //Check for errors
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

  //Handle the errors
  if(!compiled) {
    GLint info_len = 0;
    
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);
    
    if(info_len > 1) {
      char* info_log = new char[sizeof(char) * info_len];

      glGetShaderInfoLog(shader, info_len, NULL, info_log);
      std::cerr << "ERROR: SHADER LOADING " << std::endl  << info_log << std::endl;
      delete []info_log;
    }
    glDeleteShader(shader);
    return 0;
  }
  return shader;
}



GLuint shader_create(const string vs, const string fs) {
  GLuint vertex_shader;
  GLuint fragment_shader;
  GLint linked;

  //Load the fragment and vertex shaders
  vertex_shader = load_shader(GL_VERTEX_SHADER, vs);
  fragment_shader = load_shader(GL_FRAGMENT_SHADER, fs);

  //Create the program object
  program_obj = glCreateProgram();

  if(program_obj == 0) {
    cerr << "ERROR FLAG: " << glGetError();
    std::cerr << "ERROR: SHADER PROGRAM CREATION. Could not create program object." << std::endl;
    return 0;
  }
  cout << "PROGRAM: " << program_obj << endl;
  glAttachShader(program_obj, vertex_shader);
  glAttachShader(program_obj, fragment_shader);


  glBindAttribLocation(program_obj, VERTEX_POS_INDX, "a_position");
  glBindAttribLocation(program_obj, VERTEX_TEXCOORD0_INDX, "a_texCoord");

  //Link the program
  glLinkProgram(program_obj);

  //Check to see if we have any log info
  glGetProgramiv(program_obj, GL_LINK_STATUS, &linked);
  
  if(!linked) {
    GLint info_len = 0;
    
    glGetProgramiv(program_obj, GL_INFO_LOG_LENGTH, &info_len);

    if(info_len > 1) {
      char* info_log = new char[sizeof(char)*info_len];

      glGetProgramInfoLog(program_obj, info_len, NULL, info_log);
      std::cerr << "ERROR: PROGRAM LINKING " << std::endl  << info_log << std::endl;
      delete []info_log;
    }
    glDeleteProgram(program_obj);
    return 0;
  }
  
  return program_obj;
}


/***********************************************************
 * Name: load_tex_images
 *
 * Arguments:
 *       void
 *
 * Description: Loads three raw images to use as textures on faces
 *
 * Returns: void
 *
 ***********************************************************/

static void load_tex_images()
{
  FILE *tex_file1, *tex_file2 = NULL;
  int bytes_read, image_sz = IMAGE_SIZE_WIDTH*IMAGE_SIZE_HEIGHT*3;

   tex_buf1 = new char[image_sz];

   tex_file1 = fopen(PATH "../resources/Djenne_128_128.raw", "rb");
   if(tex_file1 == NULL) {
     std::cerr << "ERROR: Couldn't load textures" << endl;
   }

   if (tex_file1 && tex_buf1)
   {
      bytes_read=fread(tex_buf1, 1, image_sz, tex_file1);
      assert(bytes_read == image_sz);  // some problem with file?
      fclose(tex_file1);
   }


   // tex_buf2 = new char[image_sz];

   // tex_file2 = fopen(PATH "../graphics/tiles/assets/characters_1.raw", "rb");
   // if(tex_file1 == NULL) {
   //   std::cerr << "ERROR: Couldn't load textures" << endl;
   // }

   // if (tex_file1 && tex_buf1)
   // {
   //    bytes_read=fread(tex_buf1, 1, image_sz, tex_file1);
   //    assert(bytes_read == image_sz);  // some problem with file?
   //    fclose(tex_file1);
   // }

}
  

/***********************************************************
 * Name: init_textures
 *
 * Description:   Initialise OGL|ES texture surfaces to use image
 *                buffers
 *
 * Returns: void
 *
 ***********************************************************/
static void init_textures()
{
  
  //  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  load_tex_images();
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, IMAGE_SIZE_WIDTH, IMAGE_SIZE_HEIGHT, 0,
                GL_RGB, GL_UNSIGNED_BYTE, tex_buf1);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST);

}


//------------------------------------------------------------------------------

static void exit_func(void)
// Function to be passed to atexit().
{
   // clear screen
   glClear( GL_COLOR_BUFFER_BIT );
   
   // release buffers
   delete []tex_buf1;
   delete []mapData;
   delete []sprite_data;
   delete []sprite_tex_data;
   delete []mapTexCoords;
   delete []tileSetTexCoords;

   printf("\nClosed\n");
} // exit_func()


static float get_dt() {

   static std::chrono::steady_clock::time_point curr_time = std::chrono::steady_clock::now();


   std::chrono::steady_clock::time_point milliseconds = std::chrono::steady_clock::now();   

typedef std::chrono::duration<int, std::milli> millisecs_t;
millisecs_t duration(std::chrono::duration_cast<millisecs_t>(milliseconds - curr_time));

 curr_time = milliseconds;
return duration.count()/ 1000.0f;
   // //Hold the current time
   // static double curr_seconds = 0.0f; //seconds

  
   // //Get the current time
   // struct timeval time_data;
   // gettimeofday(&time_data, NULL);
   // double seconds = time_data.tv_sec + time_data.tv_usec /1000000.0f;

   // //If we've just initialised, then we don't have a previous time - set it and return 0
   // if(curr_seconds == 0.0f) {
   //   curr_seconds = seconds;
   //   return 0.0f;
   // }
   

   // //   cout << "SEC " << seconds << " CURR " << curr_seconds << endl;
   // //Calculate the time difference
   // float dt = seconds - curr_seconds;
   // if(dt < 0.0f) {
   //   return 0.0f;
   // }
   // //update the previous time
   // curr_seconds = seconds;
   // return dt;

}

bool init_shaders() {
#ifdef USE_GLES
  //read in the shaders
  ifstream vertex_shader_src("vert_shader.glesv");
  ifstream fragment_shader_src("frag_shader.glesf");
#endif
#ifdef USE_GL
  //read in the shaders
  ifstream vertex_shader_src("vert_shader.glv");
  ifstream fragment_shader_src("frag_shader.glf");

#endif
  if(!vertex_shader_src.good()){
    std::cerr << "Failed to load vertex shader" << endl;
    return false;
  }
    
  if(!fragment_shader_src.good() ){
    cerr << "Failed to load fragment shader" << endl;
    return false;
  }

  std::string vert_src, frag_src, line;
  while(getline(vertex_shader_src, line)) 
    vert_src += line + "\n";

  while(getline(fragment_shader_src, line))
    frag_src += line + "\n";

  GLuint program_obj = shader_create(vert_src, frag_src);

  if(program_obj == 0){
    std::cout << "Failed to create the shader" << std::endl;
    return false;
  }

  return true;
}
void generate_sprite_tex_data() {

  //holds the map data
  //need 12 float for the 2D texture coordinates
  int num_floats = 12;
  sprite_tex_data = new GLfloat[sizeof(GLfloat)*num_floats]; 
  assert(sprite_tex_data != 0);

  //generate the map data

  int currTile = 13;
  GLfloat *tileSetPtr = &tileSetTexCoords[currTile*8];
  //bottom left
  sprite_tex_data[0] = tileSetPtr[0];
  sprite_tex_data[1] = tileSetPtr[1];

  //top left
  sprite_tex_data[2] = tileSetPtr[2];
  sprite_tex_data[3] = tileSetPtr[3];

  //bottom right
  sprite_tex_data[4] = tileSetPtr[4];
  sprite_tex_data[5] = tileSetPtr[5];

  //top left
  sprite_tex_data[6] = tileSetPtr[2];
  sprite_tex_data[7] = tileSetPtr[3];

  //top right
  sprite_tex_data[8] = tileSetPtr[6];
  sprite_tex_data[9] = tileSetPtr[7];
	
  //bottom right
  sprite_tex_data[10] = tileSetPtr[4];
  sprite_tex_data[11] = tileSetPtr[5];

}

void generate_sprite_coords() {
  #ifdef DEBUG
  printf("GENERATING MAP DATA...");
#endif
  //holds the map data
   //need 18 floats for each coordinate as these hold 3D coordinates
  int num_floats = 18;
  sprite_data  = new GLfloat[sizeof(GLfloat)*num_floats]; 
  assert(sprite_data != 0);
  float scale = 16.0f;
  //generate the map data
  /**
   * Vertex winding order:
   * 1, 3   4
   *  * --- * 
   *  |     |
   *  |     | 
   *  * --- *
   * 0       2,5
   */

  //generate one tile's worth of data
  
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


#ifdef DEBUG
  printf("DONE.");
#endif


}

//==============================================================================

int main ()
{
   GameWindow window(640, 480, false);
   window.use_context();

   // Start OGLES
   init_ogl();

   if(!init_shaders())
     return 0;

   generate_tileset_coords(128, 128);
   generate_map_texcoords(map_width, map_height);
   generate_sprite_coords();
   generate_sprite_tex_data();

   // initialise the OGLES texture(s)
   init_textures();

   generate_map_coords(map_width, map_height);
   init_buffers();
   objects[0].x = 330.0f;
   objects[0].y = 330.0f;
   objects[1].x = 300.0f;
   objects[1].y = 300.0f;
   // Setup the model world
   //   init_model_proj(&window);

   std::thread mythread(run_all);

   float dt = get_dt();
   int count = 0;
   while (!window.check_close())
   {
     init_model_proj(&window);
     //Get the time since the last iteration 
     dt = get_dt(); 
     count++;
     update(dt);
     redraw_scene(&window, dt);
     GameWindow::update();
//     if(count == 100) break;
   }
   exit_func();
   mythread.join();
   return 0;
}

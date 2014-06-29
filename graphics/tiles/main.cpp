/*
Copyright (c) 2012, Broadcom Europe Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
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
 //Include GLM
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "bcm_host.h"

#include "GLES/gl.h"
#include "GLES2/gl2.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

#include "main.h"

#define PATH "./"

#define IMAGE_SIZE 128

#ifndef M_PI
#define M_PI 3.141592654
#endif
using namespace std;	

typedef struct
{
  uint32_t screen_width;
  uint32_t screen_height;
  // OpenGL|ES objects
  EGLDisplay display;
  EGLSurface surface;
  EGLContext context;
  GLuint tex[6];
  // model rotation vector and direction
  GLfloat rot_angle_x_inc;
  GLfloat rot_angle_y_inc;
  GLfloat rot_angle_z_inc;
  // current model rotation angles
  GLfloat rot_angle_x;
  GLfloat rot_angle_y;
  GLfloat rot_angle_z;
  // current distance from camera
  GLfloat distance;
  GLfloat distance_inc;
  // pointers to texture buffers
  char *tex_buf1;
  char *tex_buf2;
  char *tex_buf3;
} CUBE_STATE_T;

static  void init_ogl(CUBE_STATE_T *state);
static void init_model_proj(CUBE_STATE_T *state);
static void reset_model(CUBE_STATE_T *state);
static GLfloat inc_and_wrap_angle(GLfloat angle, GLfloat angle_inc);
static GLfloat inc_and_clip_distance(GLfloat distance, GLfloat distance_inc);
static void redraw_scene(CUBE_STATE_T *state,float dt);
static void update_model(CUBE_STATE_T *state, float dt);
static void init_textures(CUBE_STATE_T *state);
static void load_tex_images(CUBE_STATE_T *state);
static void exit_func(void);
static volatile int shutdown;
static CUBE_STATE_T _state, *state=&_state;
#define VERTEX_POS_INDX 0
#define VERTEX_TEXCOORD0_INDX 1
static void draw_map(int image_height, int image_width, float dt);
GLuint vboIds[2];
GLuint program_obj =0;
GLuint texture_id = 0;
const int map_height = 16;
const int map_width = 16;


glm::mat4 projection_matrix;

static void init_buffers() {
  glUseProgram(program_obj);

  //vboIds[0] = position
  //vboIds[1] = texture 0
  glGenBuffers(2, vboIds);
  //  cout << "HERE: " << hex<< glGetError() << endl;
  glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
  glBufferData(GL_ARRAY_BUFFER, map_height*map_width*sizeof(GLfloat)*18, mapData, GL_STATIC_DRAW);
  

  glBindBuffer(GL_ARRAY_BUFFER, vboIds[1]);
  glBufferData(GL_ARRAY_BUFFER, map_height*map_width*sizeof(GLfloat)*12, mapTexCoords, GL_STATIC_DRAW);
  //  cout << "CODE: " << endl << glGetError() << endl;
  

  glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
  glEnableVertexAttribArray(VERTEX_POS_INDX);
  glBindBuffer(GL_ARRAY_BUFFER, vboIds[1]);
  glEnableVertexAttribArray(VERTEX_TEXCOORD0_INDX);
  //  cout << "CODE: " << endl << glGetError() << endl;
  glVertexAttribPointer(VERTEX_POS_INDX, 3, GL_FLOAT, GL_FALSE,0, 0);
  glVertexAttribPointer(VERTEX_TEXCOORD0_INDX, 2, GL_FLOAT, GL_FALSE, 0, 0);
  //  cout << "HERE NOW : " << hex<< glGetError() << endl;


  //  cout << "HERE NOW : " << hex<< glGetError() << endl;
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,texture_id);

  //p  cout << "CODE: " << endl << glGetError() << endl;


}

float map_scroll_speed = 1.0f; //1 tile a second
float map_bottom_x = 0.0;
float map_bottom_y = 0.0; 
const int map_display_width = 8;
const int map_display_height = 8;
static void animate(float dt) {
  float map_display_right_x = map_bottom_x + map_display_width;
  float map_display_top_y = map_bottom_y + map_display_height;

  //scroll the map towards the top right
  map_bottom_x += map_scroll_speed*dt;
  map_bottom_y += map_scroll_speed*dt;

  //perform wrapping
  if(map_display_right_x > map_width+ 20.0f) {
    map_bottom_x = 0.0f;//wrap round
  }

  if(map_display_top_y > map_height+ 20.0f) {
    map_bottom_y = 0.0f; //wrap round
  }
}
static void draw_map(int map_width, int map_height, float dt)
{

  //Draw the tiles
  //TODO, improve efficency
  //  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  //genera  data 
  //  glEnableVertexAttribArray(VERTEX_POS_INDX);
  //glVertexAttribPointer(VERTEX_POS_INDX, 3, GL_FLOAT, GL_FLASE, 0, mapData);
  //  glEnableClientState( GL_VERTEX_ARRAY );
  //     glVertexPointer(3, GL_FLOAT, 0, mapData );
  glDisable(GL_CULL_FACE);
  //            glEnable(GL_CULL_FACE);
     //   glDrawArrays(GL_TRIANGLES, 0, map_height*map_width*6;
  //  glEnableClientState(GL_VERTEX_ARRAY);
  animate(dt);

  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 translated = glm::translate(model, glm::vec3(map_bottom_x, map_bottom_y, 0.0f));

  //i  glTranslatef(map_bottom_x, map_bottom_y, 0);
  //  glTranslatef(-10.0f, 0.0f, 0.0f);
  glUseProgram(program_obj);
  //  glLoadIdentity();
  //Get modelview and projection matrices
  //  cout << "ERROR: " << hex << glGetError() << endl;
  //  glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);
  //  cout << "ERROR: " << hex << glGetError() << endl;
  //  for(int i = 0; i < 16; i++){
  //    if( i % 4 == 0) cout << endl;
    //    cout << modelview_matrix[i] << " ";
  //  }
  //  cout << "NEW ERROR: " << hex << glGetError() << endl;
  //    glGetFloatv(GL_PROJECTION_MATRIX, projection_matrix);

    
  glUniformMatrix4fv(glGetUniformLocation(program_obj, "mat_projection"), 1, GL_FALSE,glm::value_ptr(projection_matrix));
  glUniformMatrix4fv(glGetUniformLocation(program_obj, "mat_modelview"), 1, GL_FALSE, glm::value_ptr(translated));

  //  cout << "UNIFORM" << glGetUniformLocation(program_obj, "mat_projection")<< endl;
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
  //  cout << endl << "DT: " << dt << endl;

	  //	  glBindAttribLocation(program_obj, 1, "

	  //	  glDrawArrays(GL_TRIANGLES, 0, 3);
	  /*
	  int x, y;
  int count = 0;
  for(x = 0; x < map_width; x++)
    {
      for(y = 0; y < map_height; y++)
	{	  

	  // draw first 4 vertices
	       	  glDrawArrays( GL_TRIANGLE_STRIP, count,4);
	  	  count += 4;

	}
	}*/
 
}

/***********************************************************
 * Name: init_ogl
 *
 * Arguments:
 *       CUBE_STATE_T *state - holds OGLES model info
 *
 * Description: Sets the display, OpenGL|ES context and screen stuff
 *
 * Returns: void
 *
 ***********************************************************/
static void init_ogl(CUBE_STATE_T *state)
{
  int32_t success = 0;
  EGLBoolean result;
  EGLint num_config;

  static EGL_DISPMANX_WINDOW_T nativewindow;

  DISPMANX_ELEMENT_HANDLE_T dispman_element;
  DISPMANX_DISPLAY_HANDLE_T dispman_display;
  DISPMANX_UPDATE_HANDLE_T dispman_update;
  VC_RECT_T dst_rect;
  VC_RECT_T src_rect;

  static const EGLint attribute_list[] =
    {
      EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, //We want an Opengl ES 2.0 context
      EGL_RED_SIZE, 8,
      EGL_GREEN_SIZE, 8,
      EGL_BLUE_SIZE, 8,
      EGL_ALPHA_SIZE, 8,
      EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
      EGL_NONE
    };
   
  EGLConfig config;

  // get an EGL display connection
  state->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  assert(state->display!=EGL_NO_DISPLAY);
  //  setEGLContextClientVersion(2);
  // initialize the EGL display connection
  result = eglInitialize(state->display, NULL, NULL);
  assert(EGL_FALSE != result);

  // get an appropriate EGL frame buffer configuration
  result = eglChooseConfig(state->display, attribute_list, &config, 1, &num_config);
  assert(EGL_FALSE != result);
  

  //We want an Opengl ES 2 context
  EGLint attribute_list_context[] =
    {
      EGL_CONTEXT_CLIENT_VERSION, 2,
      EGL_NONE
    };
  
  // create an EGL rendering context
  state->context = eglCreateContext(state->display, config, EGL_NO_CONTEXT, attribute_list_context);
  assert(state->context!=EGL_NO_CONTEXT);

  // create an EGL window surface
  success = graphics_get_display_size(0 /* LCD */, &state->screen_width, &state->screen_height);
  assert( success >= 0 );

  dst_rect.x = 0;
  dst_rect.y = 0;
  dst_rect.width = state->screen_width;
  dst_rect.height = state->screen_height;
      
  src_rect.x = 0;
  src_rect.y = 0;
  src_rect.width = state->screen_width << 16;
  src_rect.height = state->screen_height << 16;        

  dispman_display = vc_dispmanx_display_open( 0 /* LCD */);
  dispman_update = vc_dispmanx_update_start( 0 );
         
  dispman_element = vc_dispmanx_element_add ( dispman_update, dispman_display,
					      0/*layer*/, &dst_rect, 0/*src*/,
					      &src_rect, DISPMANX_PROTECTION_NONE, 0 /*alpha*/, 0/*clamp*/, DISPMANX_NO_ROTATE/*transform*/);
      
  nativewindow.element = dispman_element;
  nativewindow.width = state->screen_width;
  nativewindow.height = state->screen_height;
  vc_dispmanx_update_submit_sync( dispman_update );
      
  state->surface = eglCreateWindowSurface( state->display, config, &nativewindow, NULL );
  assert(state->surface != EGL_NO_SURFACE);

  // connect the context to the surface
  result = eglMakeCurrent(state->display, state->surface, state->surface, state->context);
  assert(EGL_FALSE != result);

  // Set background color and clear buffers
  glClearColor(0.15f, 0.25f, 0.35f, 1.0f);

  // Enable back face culling.
  glEnable(GL_CULL_FACE);

  glMatrixMode(GL_MODELVIEW);
}

/***********************************************************
 * Name: init_model_proj
 *
 * Arguments:
 *       CUBE_STATE_T *state - holds OGLES model info
 *
 * Description: Sets the OpenGL|ES model to default values
 *
 * Returns: void
 *
 ***********************************************************/

static void init_model_proj(CUBE_STATE_T *state)
{

  glViewport(0, 0, (GLsizei)state->screen_width, (GLsizei)state->screen_height);

  projection_matrix = glm::ortho(0.0f, static_cast<float>(state->screen_width), 0.0f, static_cast<float>(state->screen_height), -1.0f, 1.0f);

   
  //  glEnableClientState( GL_VERTEX_ARRAY );
  //  glVertexPointer(3, GL_SHORT, 0, mapData );

  reset_model(state);
}

/***********************************************************
 * Name: reset_model
 *
 * Arguments:
 *       CUBE_STATE_T *state - holds OGLES model info
 *
 * Description: Resets the Model projection and rotation direction
 *
 * Returns: void
 *
 ***********************************************************/
static void reset_model(CUBE_STATE_T *state)
{
  // reset model position
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

}

/***********************************************************
 * Name: update_model
 *
 * Arguments:
 *       CUBE_STATE_T *state - holds OGLES model info
 *
 * Description: Updates model projection to current position/rotation
 *
 * Returns: void
 *
 ***********************************************************/
static void update_model(CUBE_STATE_T *state, float dt)
{

  glLoadIdentity();
}

/***********************************************************
 * Name: inc_and_wrap_angle
 *
 * Arguments:
 *       GLfloat angle     current angle
 *       GLfloat angle_inc angle increment
 *
 * Description:   Increments or decrements angle by angle_inc degrees
 *                Wraps to 0 at 360 deg.
 *
 * Returns: new value of angle
 *
 ***********************************************************/
static GLfloat inc_and_wrap_angle(GLfloat angle, GLfloat angle_inc)
{
  
  return angle;
}

/***********************************************************
 * Name: inc_and_clip_distance
 *
 * Arguments:
 *       GLfloat distance     current distance
 *       GLfloat distance_inc distance increment
 *
 * Description:   Increments or decrements distance by distance_inc units
 *                Clips to range
 *
 * Returns: new value of angle
 *
 ***********************************************************/
static GLfloat inc_and_clip_distance(GLfloat distance, GLfloat distance_inc)
{

  return distance;
}

/***********************************************************
 * Name: redraw_scene
 *
 * Arguments:
 *       CUBE_STATE_T *state - holds OGLES model info
 *
 * Description:   Draws the model and calls eglSwapBuffers
 *                to render to screen
 *
 * Returns: void
 *
 ***********************************************************/
static void redraw_scene(CUBE_STATE_T *state, float dt)
{
  // Start with a clear screen
  glClear( GL_COLOR_BUFFER_BIT );

  // Draw first (front) face:
  // Bind texture surface to current vertices
  //glBindTexture(GL_TEXTURE_2D, state->tex[0]);
  
  draw_map(16, 16, dt);

  eglSwapBuffers(state->display, state->surface);
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
  tileSetTexCoords = (GLfloat *) malloc(sizeof(GLfloat)* numTilesX * numTilesY * 4 * 2);
  assert(tileSetTexCoords != 0);
  //TODO free this memory

  double tileSetXOffset = 0.0;
  double tileSetYOffset = 0.0;
  double tileSetXInc = 1.0 / (double)numTilesX;
  double tileSetYInc = 1.0 / (double)numTilesY;
  //TODo: DIV ZEro HERRE 

  //TODO: REMEMBER TILESET COORDINATES ARE INVERSE OF IMAGE FILE ONES
  int x;
  int y;
  //generate the coordinates for each tile
  for(x = 0; x < numTilesX; x++)
    {
      for(y = 0; y< numTilesY; y++)
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
  mapTexCoords = (GLfloat *)malloc(sizeof(GLfloat)*map_height*map_width*num_floats); 
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
 * 2        3, 5
 *    * --- * 
 *    |    /|
 *    |   / |
 *    | /   |
 *    *     |
 * --- *
 * 1, 4      6 
 *
 * We want the same winding order for the triangle geometry
 ***********************************************************/


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
  mapData = (GLfloat *)malloc(sizeof(GLfloat)*map_height*map_width*num_floats); 
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
    return NULL;
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
    return NULL;
  }
   cout << "PROGRAM IS RETURN HERE " << program_obj << endl;
  //return program
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
static void load_tex_images(CUBE_STATE_T *state)
{
   FILE *tex_file1 = NULL;
   int bytes_read, image_sz = IMAGE_SIZE*IMAGE_SIZE*3;

   state->tex_buf1 =(char *) malloc(image_sz);

   tex_file1 = fopen(PATH "Lucca_128_128.raw", "rb");
   if (tex_file1 && state->tex_buf1)
   {
      bytes_read=fread(state->tex_buf1, 1, image_sz, tex_file1);
      assert(bytes_read == image_sz);  // some problem with file?
      fclose(tex_file1);
   }
}
  

/***********************************************************
 * Name: init_textures
 *

 * Arguments:
 *       CUBE_STATE_T *state - holds OGLES model info
 *
 * Description:   Initialise OGL|ES texture surfaces to use image
 *                buffers
 *
 * Returns: void
 *
 ***********************************************************/
static void init_textures(CUBE_STATE_T *state)
{
  
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  load_tex_images(state);
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, IMAGE_SIZE, IMAGE_SIZE, 0,
                GL_RGB, GL_UNSIGNED_BYTE, state->tex_buf1);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST);

   // setup overall texture environment
   //glTexCoordPointer(2, GL_FLOAT, 0, mapTexCoords);
   //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   
   //glEnable(GL_TEXTURE_2D);
   /*
   // load three texture buffers but use them on six OGL|ES texture surfaces
   load_tex_images(state);
   glGenTextures(6, &state->tex[0]);

   // setup first texture
   glBindTexture(GL_TEXTURE_2D, state->tex[0]);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, IMAGE_SIZE, IMAGE_SIZE, 0,
                GL_RGB, GL_UNSIGNED_BYTE, state->tex_buf1);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST);

   // setup overall texture environment
   glTexCoordPointer(2, GL_FLOAT, 0, mapTexCoords);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   
   glEnable(GL_TEXTURE_2D);
   */
}

//------------------------------------------------------------------------------

static void exit_func(void)
// Function to be passed to atexit().
{
   // clear screen
   glClear( GL_COLOR_BUFFER_BIT );
   eglSwapBuffers(state->display, state->surface);

   // Release OpenGL resources
   eglMakeCurrent( state->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
   eglDestroySurface( state->display, state->surface );
   eglDestroyContext( state->display, state->context );
   eglTerminate( state->display );

   // release texture buffers
   free(state->tex_buf1);

   printf("\nClosed\n");
} // exit_func()


static float get_dt() {

  //Hold the current time
   static float    curr_seconds = 0.0f; //seconds

   //Get the current time
   //   time_t    s;  //seconds
   //   struct timespec spec;

   //   clock_gettime(CLOCK_REALTIME, &spec);
   
   //   s = spec.tv_sec;
   //   float seconds = spec.tv_nsec / (float)1.0e9; // Convert nanoseconds to seconds
   struct timeval time_data;
   gettimeofday(&time_data, NULL);
   float seconds = time_data.tv_usec /1000000.0f;

   //If we've just initialised, then we don't have a previous time - set it and return 0
   if(curr_seconds = 0.0f) {
     curr_seconds = seconds;
     return 0.0f;
   }
   
   //Calculate the time difference
   float dt = seconds - curr_seconds;
   if(dt < 0.0f)
     return 0.0f;
   
   //update the previous time
   curr_seconds = seconds;
   return dt;

}
bool init_shaders() {
  //read in the shaders
  ifstream vertex_shader_src("vert_shader.cpp");
  ifstream fragment_shader_src("frag_shader.cpp");

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
   cout << "PROGRAM IS DEFFO HERE " << program_obj << endl;
  if(program_obj == 0){
    std::cout << "Failed to create the shader" << std::endl;
    return false;
  }
   cout << "PROGRAM IS DEFFO HERE " << program_obj << endl;
  return true;
  
}
//==============================================================================

int main ()
{
   bcm_host_init();

   // Clear application state
   memset( state, 0, sizeof( *state ) );

   // Start OGLES
   init_ogl(state);
     cout << "CODE: " << glGetError() << endl;

   if(!init_shaders())
     return 0;


   cout << "PROGRAM C DEFFO HERE " << program_obj << endl;
   cout << "shaders done";
   generate_tileset_coords(128, 128);
   generate_map_texcoords(map_width, map_height);

   // initialise the OGLES texture(s)
   init_textures(state);
   cout << "CODE: " << hex << glGetError() << endl;
   generate_map_coords(map_width, map_height);
   init_buffers();

   // Setup the model world
   init_model_proj(state);
   int count = 0;
   float dt = get_dt();

   while (!shutdown)
   {
     //Get the time since the last iteration 
     dt = get_dt(); 
     //         printf("\n%f\n",dt);
                   update_model(state, dt);
        redraw_scene(state, dt);
	//		count++; 
		//		if(count == 1000) shutdown = 1;
   }
   exit_func();
   return 0;
}

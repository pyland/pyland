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
    
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>

#include "bcm_host.h"

#include "GLES/gl.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

#include "cube_texture_and_coords.h"

#define PATH "./"

#define IMAGE_SIZE 128

#ifndef M_PI
   #define M_PI 3.141592654
#endif
	

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
static void redraw_scene(CUBE_STATE_T *state);
static void update_model(CUBE_STATE_T *state);
static void init_textures(CUBE_STATE_T *state);
static void load_tex_images(CUBE_STATE_T *state);
static void exit_func(void);
static volatile int terminate;
static CUBE_STATE_T _state, *state=&_state;

static void draw_map(int image_height, int image_width);


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

   // initialize the EGL display connection
   result = eglInitialize(state->display, NULL, NULL);
   assert(EGL_FALSE != result);

   // get an appropriate EGL frame buffer configuration
   result = eglChooseConfig(state->display, attribute_list, &config, 1, &num_config);
   assert(EGL_FALSE != result);

   // create an EGL rendering context
   state->context = eglCreateContext(state->display, config, EGL_NO_CONTEXT, NULL);
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
      
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrthof(0, state->screen_width, 0, state->screen_height, -1, 1);
   
   glEnableClientState( GL_VERTEX_ARRAY );
   glVertexPointer(3, GL_SHORT, 0, mapData );

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
static void update_model(CUBE_STATE_T *state)
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
static void redraw_scene(CUBE_STATE_T *state)
{
   // Start with a clear screen
   glClear( GL_COLOR_BUFFER_BIT );

   // Draw first (front) face:
   // Bind texture surface to current vertices
   glBindTexture(GL_TEXTURE_2D, state->tex[0]);

   draw_map(8, 8);

   eglSwapBuffers(state->display, state->surface);
}


/***********************************************************
 * Name: generate_tilset_coords
 *
 ***********************************************************/
static void generate_tileset_coords(int image_height, int image_width)
{
  LOG(INFO) << "Generating tileset texture coords";

  //check the tilset image height and widths are multiples of the tiles
  //  assert(image_height % TILESET_ELEMENT_SIZE != 0 || image_width % TILESET_ELEMENT_SIZE != 0);
    

  int numTilesX = image_height/ TILESET_ELEMENT_SIZE;
  int numTilesY = image_width / TILESET_ELEMENT_SIZE;

  //Each tile needs 8 floats to describe its position in the image
  tileSetTexCoords = (GLfloat *) malloc(sizeof(GLfloat)* numTilesX * numTilesY * 4 * 2);
  assert(tileSetTexCoords != 0);
  //TODO free this memory

  double tileSetXOffset = 0.0;
  double tileSetYOffset = 0.0;
  double tileSetXInc = 1.0 / numTilesX;
  double tileSetYInc = 1.0 / numTilesY;


  //TODO: REMEMBER TILESET COORDINATES ARE INVERSE OF IMAGE FILE ONES
  int x;
  int y;
  //generate the coordinates for each tile
  for(x = 0; x < numTilesX; x++)
    {
      for(y = 0; y< numTilesY; y++)
	{
	  tileSetTexCoords[x* numTilesY*4*2+y*(4*2)] =  tileSetXOffset;
	  tileSetTexCoords[x* numTilesY*4*2+y*4*2 +1] =  tileSetYOffset;

	  tileSetTexCoords[x* numTilesY*4*2+y*4*2+ 2] =  tileSetXOffset;
	  tileSetTexCoords[x* numTilesY*4*2+y*4*2+3] =  tileSetYOffset + tileSetYInc;

	  tileSetTexCoords[x* numTilesY*4*2+y*4*2+4] =  tileSetXOffset + tileSetXInc;
	  tileSetTexCoords[x* numTilesY*4*2+y*4*2+5] =  tileSetYOffset;

	  tileSetTexCoords[x* numTilesY*4*2+y*4*2+6] =  tileSetXOffset + tileSetXInc;
	  tileSetTexCoords[x* numTilesY*4*2+y*4*2+7] =  tileSetYOffset + tileSetYInc;

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
  LOG(INFO) << "Generating map texture data";
  //holds the map data
  //need 8 float for the 2D texture coordinates
  mapTexCoords = (GLfloat *)malloc(sizeof(GLfloat)*map_height*map_width*8); 
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
	  mapTexCoords[x*map_height*8 + y*8] = *(tileSetPtr);
	  mapTexCoords[x*map_height+8 + y*8+1] = *(tileSetPtr+1);
	  //bottom right
	  mapTexCoords[x*map_height*8 + y*8+2] = *(tileSetPtr+4);
	  mapTexCoords[x*map_height*8 + y*8+3] = *(tileSetPtr+5);

	  //top left
	  mapTexCoords[x*map_height*8 + y*8+4] = *(tileSetPtr+2);
	  mapTexCoords[x*map_height*8 + y*8+5] = *(tileSetPtr+3);

	  //top right
	  mapTexCoords[x*map_height*8 + y*8+6] = *(tileSetPtr+6);
	  mapTexCoords[x*map_height*8 + y*8+7] = *(tileSetPtr+7);
	  
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
  LOG(INFO) << "Generating map data";

  //holds the map data
  //need 12 shorts for each coordinate as these hold 3D coordinates
  mapData = (GLfloat *)malloc(sizeof(GLfloat)*map_height*map_width*6*2); 
  assert(mapData != 0);
  float scale = 100.0f;
  //generate the map data
  int x, y;
  for(x = 0; x < map_width; x++)
    {
      for(y = 0; y < map_height; y++)
	{
	  //bottom left
	    mapData[x*map_height*12 + y*12] = x * scale;
	    mapData[x*map_height*12 + y*12+1] = y * scale;
	    mapData[x*map_height*12 + y*12+2] = 0;
	    

	    //bottom right
	    mapData[x*map_height*12 + y*12+3] = (x+1) * scale;
	    mapData[x*map_height*12 + y*12+4] = y * scale;
	    mapData[x*map_height*12 + y*12+5] = 0;
	    
	    //top left
	    mapData[x*map_height*12 + y*12+6] = x * scale;
	    mapData[x*map_height*12 + y*12+7] = (y+1) * scale;
	    mapData[x*map_height*12 + y*12+8] = 0;
	    
	    //top right
	    mapData[x*map_height*12 + y*12+9] = (x+1) * scale;
	    mapData[x*map_height*12 + y*12+10] = (y+1) * scale;
	    mapData[x*map_height*12 + y*12+11] = 0;
	    	}
    }
}

static void draw_map(int map_width, int map_height)
{
 
  //Draw the tiles
 //TODO, improve efficency

 //generate the map data 
  //  glEnableVertexAttribArray(VERTEX_POS_INDX);
  //glVertexAttribPointer(VERTEX_POS_INDX, 3, GL_FLOAT, GL_FLASE, 0, mapData);
       glEnableClientState( GL_VERTEX_ARRAY );
     glVertexPointer(3, GL_FLOAT, 0, mapData );
     //   glDisable(GL_CULLFING);

     //   glDrawArrays(GL_TRIANGLES, 0, map_height*map_width*6;
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
}

int main ()
{
   bcm_host_init();

   // Clear application state
   memset( state, 0, sizeof( *state ) );
      
   // Start OGLES
   init_ogl(state);

   
   
generate_tileset_coords(320, 320);
 generate_map_texcoords(8, 8);


   // initialise the OGLES texture(s)
   init_textures(state);

 generate_map_coords(8, 8);

   // Setup the model world
   init_model_proj(state);

   while (!terminate)
   {
      update_model(state);
      redraw_scene(state);
   }
   exit_func();
   return 0;
}

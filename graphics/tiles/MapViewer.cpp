    
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


MapViewer::MapViewer() {

}

MapViewer::~MapViewer() {

}
/**
 * This function generates the texture coordinates for each tile in
 * the tileset, caching them so that we can quickly generate the map's
 * texture coordinates, and update them, if we need to. 
 */
void MapViewer::generate_tileset_coords() {
  //Stop division by 0
  assert(TILESET_ELEMENT_SIZE < 1);

  int numTilesX = tileset_width/ TILESET_ELEMENT_SIZE;
  int numTilesY = tileset_height / TILESET_ELEMENT_SIZE;

  //Each tile needs 8 floats to describe its position in the image, 4
  //vertices and then 2 floats for each one.
  tileSetTexCoords = (GLfloat *) new[sizeof(GLfloat)* numTilesX * numTilesY * 4 * 2];
  assert(tileSetTexCoords != 0);
  //TODO free this memory

  //Stop divison by 0
  assert(numTilesX == 0);
  assert(numTilesY == 0);
  //Calculate the increments for each tile
  double tileSetXOffset = 0.0;
  double tileSetYOffset = 0.0;
  double tileSetXInc = 1.0 / (double)numTilesX;
  double tileSetYInc = 1.0 / (double)numTilesY;
  
  //TODO: REMEMBER TILESET COORDINATES ARE INVERSE OF IMAGE FILE ONES
  //Generate the coordinates for each tile
  for(int x = 0; x < numTilesX; x++)
    {
      for(int y = 0; y< numTilesY; y++)
	{
	  //bottom left
	  tileSetTexCoords[x* numTilesY*4*2+y*(4*2)] =tileSetXOffset;
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


/**
 * This code generates the texture coordinates that we use for the map
 */
void MapViewer::generate_map_texcoords() {
  //We need 8 floats for the 2D texture coordinates
  mapTexCoords = (GLfloat *)new[sizeof(GLfloat)*map_height*map_width*8]; 
  assert(mapTexCoords != 0);

  //Get the texture coordinates for the particular tile
  for(int x = 0; x < map_width; x++)
    {
      for(int y = 0; y < map_height; y++)
	{
	  //Get the pointer to the current tile data
	  int currTile = worldData[x*map_height +y];
	  GLfloat *tileSetPtr = &tileSetTexCoords[currTile*8];

	  //bottom left
	  mapTexCoords[x*map_height*8 + y*8+0] = tileSetPtr[0];
	  mapTexCoords[x*map_height*8 + y*8+1] = tileSetPtr[1];

	  //bottom right
	  mapTexCoords[x*map_height*8 + y*8+2] = tileSetPtr[4];
	  mapTexCoords[x*map_height*8 + y*8+3] = tileSetPtr[5];

	  //top left
	  mapTexCoords[x*map_height*8 + y*8+4] = tileSetPtr[2];
	  mapTexCoords[x*map_height*8 + y*8+5] = tileSetPtr[3];

	  //top right
	  mapTexCoords[x*map_height*8 + y*8+6] = tileSetPtr[6];
	  mapTexCoords[x*map_height*8 + y*8+7] = tileSetPtr[7];
	  
	}
    }  
}

/**
 * This code generates the vertex data for the map
 */
void MapViewer::generate_map_coords() {
  //Need 12 shorts for each coordinate as these hold 3D coordinates
  mapData = (GLfloat *)new[sizeof(GLfloat)*map_height*map_width*6*2]; 
  assert(mapData != 0);

  //Each tile is  32 pixels in size. But, the map coordinates are in one unit increments starting at 0
  //and each unit corresponds to one pixel. So we scale up by 32 to get 32 pixels for each tile on-screen.
  float scale = 100.0f;

  //Generate the map data
  for(int x = 0; x < map_width; x++) {
      for(int y = 0; y < map_height; y++) {
	std::vector<float> target = {
	   x    * scale,  y    * scale, 0.0,
          (x+1) * scale,  y    * scale, 0.0,
           x    * scale,  y    * scale, 0.0,
          (x+1) * scale, (y+1) * scale, 0.0
	};

	int i = 0;
        for (auto t : target) {
	  mapData[x*map_height*12 + y*12+i++] = t;
        }
      }
  }
}

/**
 * This method is responsible for rendering the map.
 */
void MapViewer::draw_map() {
  glEnableClientState( GL_VERTEX_ARRAY );
  glVertexPointer(3, GL_FLOAT, 0, mapData );

  glDisable(GL_CULL_FACE);
  //Each tile is 
  int count = 0;
  for(int x = 0; x < map_width; x++)
    {
      for(int y = 0; y < map_height; y++)
	{	  
	  // draw first 4 vertices
      	  glDrawArrays( GL_TRIANGLE_STRIP, count,4);
  	  count += 4;
	}
    }
 
}

/**
 * This method will take the provided map file, parse it and generate
 * all the required objects and data that we need to render and
 * interact with it.
 */
bool load_map(int map_width, int map_height) {

}


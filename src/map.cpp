#include "map.hpp"
#include "game_window.hpp"


#include <string>
#include <iostream>
#include <fstream>
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


#define VERTEX_POS_INDX 0
#define VERTEX_TEXCOORD0_INDX 1
#define PATH "./"
#define GLOBAL_SCALE 2

#define IMAGE1_SIZE_WIDTH 128
#define IMAGE1_NUM_COMPONENTS 4
#define IMAGE1_SIZE_HEIGHT 240


#define TILESET_ELEMENT_SIZE 16


/**
 * Constructor for Map
 */ 
Map::Map(const std::string map_src) : renderable_component() {
  glActiveTexture(GL_TEXTURE0);

  init_shaders();
  generate_tileset_coords(IMAGE1_SIZE_WIDTH, IMAGE1_SIZE_HEIGHT);
  generate_map_texcoords();
  generate_map_coords();
  //  generate_sprite_tex_data();
  init_textures();
}

/** 
 *Destructor for Map
 */
Map::~Map() {
    // release buffers
    delete[] tex_buf[0];
    delete[] map_data;
    delete[] map_tex_coords;
    delete[] tileset_tex_coords;

    std::cout << std::endl << "Closed" << std::endl;
}

/**
 * The function used to generate the cache of tile texture coordinates.
 */ 
void Map::generate_tileset_coords(int tileset_width, int tileset_height) {
#ifdef DEBUG
    printf("GENERATING TILESET TEXTURE COORDS...");
#endif
    //check the tilset image height and widths are multiples of the tiles
    //  assert(image_height % TILESET_ELEMENT_SIZE != 0 || image_width % TILESET_ELEMENT_SIZE != 0);

    assert(TILESET_ELEMENT_SIZE != 0);

    int num_tiles_x = tileset_width  / TILESET_ELEMENT_SIZE;
    int num_tiles_y = tileset_height / TILESET_ELEMENT_SIZE;
    std::cout << num_tiles_x << " " << num_tiles_y << std::endl;

    assert(num_tiles_x);
    assert(num_tiles_y);

    //Each tile needs 8 floats to describe its position in the image
    tileset_tex_coords = new GLfloat[sizeof(GLfloat)* num_tiles_x * num_tiles_y * 4 * 2];
    assert(tileset_tex_coords);

    //Tiles are indexed from top left but Openl uses texture coordinates from bottom left
    //so we remap these 
    //We work from left to right, moving down
    GLfloat tileset_offset_x = 0.0;
    GLfloat tileset_offset_y = 0.0;
    GLfloat tileset_inc_x = GLfloat(1.0) / static_cast<GLfloat>(num_tiles_x);
    GLfloat tileset_inc_y = GLfloat(1.0) / static_cast<GLfloat>(num_tiles_y);
    //TODo: DIV ZEro HERRE 

    //TODO: REMEMBER TILESET COORDINATES ARE INVERSE OF IMAGE FILE ONES
    //generate the coordinates for each tile
    for (int y = 0; y < num_tiles_y; y++) {
        for (int x = 0; x < num_tiles_x; x++) {
            //bottom left
            tileset_tex_coords[y* num_tiles_x*4*2+x*(4*2)]  = tileset_offset_x;
            tileset_tex_coords[y* num_tiles_x*4*2+x*4*2 +1] = tileset_offset_y + tileset_inc_y;
 
            //top left
            tileset_tex_coords[y* num_tiles_x*4*2+x*4*2+ 2] = tileset_offset_x;
            tileset_tex_coords[y* num_tiles_x*4*2+x*4*2+3]  = tileset_offset_y;
 
            //bottom right
            tileset_tex_coords[y* num_tiles_x*4*2+x*4*2+4] = tileset_offset_x + tileset_inc_x;
            tileset_tex_coords[y* num_tiles_x*4*2+x*4*2+5] = tileset_offset_y + tileset_inc_y;
 
            //top right
            tileset_tex_coords[y* num_tiles_x*4*2+x*4*2+6] = tileset_offset_x + tileset_inc_x;
            tileset_tex_coords[y* num_tiles_x*4*2+x*4*2+7] = tileset_offset_y;
 
            tileset_offset_x += tileset_inc_x;
            //      for(int i =0 ; i < 8; i++)
            //        cout << " " <<        tileset_tex_coords[y* num_tiles_x*4*2+x*4*2+i];
            //      cout << endl;
        }
        tileset_offset_x = 0.0;
        tileset_offset_y += tileset_inc_y;
    }
}

/**
 * The function which generates the texture coordinates for the map
 * geometry, using the cached tile coordinates.
 */
void Map::generate_map_texcoords() {
#ifdef DEBUG
    printf("GENERATING MAP TEXTURE DATA...");
#endif
    //holds the map data
    //need 12 float for the 2D texture coordinates
    int num_floats = 12;
    int data_size = sizeof(GLfloat)*map_height*map_width*num_floats;
    map_tex_coords = new GLfloat[data_size]; 
    assert(map_tex_coords);

    //generate the map data
    // get the tile set coordinates for the particular tile
    for(int x = 0; x < map_width; x++) {
        for(int y = 0; y < map_height; y++) {
            int curr_tile = world_data[x][y];

            GLfloat *tileset_ptr = &tileset_tex_coords[curr_tile*8];
            //bottom left
            map_tex_coords[x*map_height*num_floats + y*num_floats+0] = tileset_ptr[0];
            map_tex_coords[x*map_height*num_floats + y*num_floats+1] = tileset_ptr[1];

            //top left
            map_tex_coords[x*map_height*num_floats + y*num_floats+2] = tileset_ptr[2];
            map_tex_coords[x*map_height*num_floats + y*num_floats+3] = tileset_ptr[3];

            //bottom right
            map_tex_coords[x*map_height*num_floats + y*num_floats+4] = tileset_ptr[4];
            map_tex_coords[x*map_height*num_floats + y*num_floats+5] = tileset_ptr[5];

            //top left
            map_tex_coords[x*map_height*num_floats + y*num_floats+6] = tileset_ptr[2];
            map_tex_coords[x*map_height*num_floats + y*num_floats+7] = tileset_ptr[3];

            //top right
            map_tex_coords[x*map_height*num_floats + y*num_floats+8] = tileset_ptr[6];
            map_tex_coords[x*map_height*num_floats + y*num_floats+9] = tileset_ptr[7];
            
            //bottom right
            map_tex_coords[x*map_height*num_floats + y*num_floats+10] = tileset_ptr[4];
            map_tex_coords[x*map_height*num_floats + y*num_floats+11] = tileset_ptr[5];
        }
    }

    //Set this data in the renderable component
    renderable_component.set_texture_coords_data(map_tex_coords, data_size, false);

}

/*
 * The function which generates the map geometry so that it can be
 * rendered to the screen
 */
void Map::generate_map_coords() {
#ifdef DEBUG
    printf("GENERATING MAP DATA...");
#endif
    //holds the map data
    //need 18 floats for each coordinate as these hold 3D coordinates
    int num_floats = 18;
    int data_size = sizeof(GLfloat)*map_height*map_width*num_floats;
    map_data = new GLfloat[data_size]; 
    assert(map_data);
    float scale = TILESET_ELEMENT_SIZE * GLOBAL_SCALE;
    //generate the map data
    ///
    /// Vertex winding order:
    /// 1, 3   4
    ///  * --- * 
    ///  |     |
    ///  |     | 
    ///  * --- *
    /// 0       2,5
    ///
    for(int x = 0; x < map_width; x++) {
        for(int y = 0; y < map_height; y++) {
            //generate one tile's worth of data

            //bottom left
            map_data[x*map_height*num_floats + y*num_floats+ 0] = scale * float(x);
            map_data[x*map_height*num_floats + y*num_floats+ 1] = scale * float(y);
            map_data[x*map_height*num_floats + y*num_floats+ 2] = 0;
             
            //top left
            map_data[x*map_height*num_floats + y*num_floats+ 3] = scale * float(x);
            map_data[x*map_height*num_floats + y*num_floats+ 4] = scale * float(y + 1);
            map_data[x*map_height*num_floats + y*num_floats+ 5] = 0;

            //bottom right
            map_data[x*map_height*num_floats + y*num_floats+ 6] = scale * float(x + 1);
            map_data[x*map_height*num_floats + y*num_floats+ 7] = scale * float(y);
            map_data[x*map_height*num_floats + y*num_floats+ 8] = 0;
            
            //top left
            map_data[x*map_height*num_floats + y*num_floats+ 9]  = scale * float(x);
            map_data[x*map_height*num_floats + y*num_floats+10] = scale * float(y+1);
            map_data[x*map_height*num_floats + y*num_floats+11] = 0;
        
            //top right
            map_data[x*map_height*num_floats + y*num_floats+12] = scale * float(x+1);
            map_data[x*map_height*num_floats + y*num_floats+13] = scale * float(y+1);
            map_data[x*map_height*num_floats + y*num_floats+14] = 0;

            //bottom right
            map_data[x*map_height*num_floats + y*num_floats+15] = scale * float(x+1);
            map_data[x*map_height*num_floats + y*num_floats+16] = scale * float(y);
            map_data[x*map_height*num_floats + y*num_floats+17] = 0;
        }
    }

#ifdef DEBUG
    printf("DONE.");
#endif

   //Set this data in the renderable component
    renderable_component.set_vertex_data(map_data, data_size, false);
    renderable_component.set_num_vertices_render(6*map_width*map_height);
}

void Map::init_textures() {
    
    FILE *tex_file1 = nullptr;
    size_t bytes_read = 0;
    size_t image_sz_1 = IMAGE1_SIZE_WIDTH*IMAGE1_SIZE_HEIGHT*IMAGE1_NUM_COMPONENTS;

    tex_buf[0] = new char[image_sz_1];

    tex_file1 = fopen(PATH "../resources/basictiles_2.raw", "rb");
    if(tex_file1 == nullptr) {
      std::cerr << "ERROR: Couldn't load textures" << std::endl;
    }

    if (tex_file1 && tex_buf[0]) {
        bytes_read = fread(tex_buf[0], 1, image_sz_1, tex_file1);
        assert(bytes_read == image_sz_1);  // some problem with file?
        fclose(tex_file1);
    }
    //Set the texture data in the rederable component
    renderable_component.set_texture_data(tex_buf[0], static_cast<int>(image_sz_1), IMAGE1_SIZE_WIDTH, IMAGE1_SIZE_HEIGHT, false);
}

/**
 * This function initialises the shader, creating and loading them.
 */ 
bool Map::init_shaders() {
#ifdef USE_GLES
  //read in the shaders
  std::ifstream vertex_shader_src("vert_shader.glesv");
  std::ifstream fragment_shader_src("frag_shader.glesf");
#endif
#ifdef USE_GL
  //read in the shaders
  std::ifstream vertex_shader_src("vert_shader.glv");
  std::ifstream fragment_shader_src("frag_shader.glf");
#endif

  if (!vertex_shader_src.good()){
    std::cerr << "Failed to load vertex shader" << std::endl;
    return false;
  }
    
  if (!fragment_shader_src.good()) {
    std::cerr << "Failed to load fragment shader" << std::endl;
    return false;
  }

  std::string vert_src, frag_src, line;
  while (getline(vertex_shader_src, line)) {
    vert_src += line + "\n";
  }

  while (getline(fragment_shader_src, line)) {
    frag_src += line + "\n";
  }

  Shader* shader = new Shader(vert_src, frag_src);
  
  if (!shader->is_loaded()) {
    delete shader;
    shader = nullptr;
    std::cerr << "Failed to create the shader" << std::endl;
    return false;
  }

  //Set the shader
  renderable_component.set_shader(shader);

  return true;

}


/**
 * The function used to update elements on the map.
 */
void Map::update_map(float dt) {
}

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
#include "interpreter.h"
#include "Api.h"
#ifdef USE_GLES

#include <GLES2/gl2.h>

#endif

#ifdef USE_GL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif


#include "main.h"
#include "MapViewer.h"
#include "Map.h"

#define PATH "./"
#define GLOBAL_SCALE 2

#define IMAGE1_SIZE_WIDTH 128
#define IMAGE1_NUM_COMPONENTS 4
#define IMAGE1_SIZE_HEIGHT 240



#define IMAGE2_SIZE_WIDTH 192
#define IMAGE2_NUM_COMPONENTS 4
#define IMAGE2_SIZE_HEIGHT 128

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



static volatile int shutdown;

#define VERTEX_POS_INDX 0
#define VERTEX_TEXCOORD0_INDX 1
const int num_vbo_ids = 4;
static void draw_map(int image_height, int image_width, float dt);
GLuint vboIds[num_vbo_ids];
GLuint program_obj =0;
GLuint texture_ids[2];
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
char* tex_buf2;
glm::mat4 projection_matrix;
const int num_objects = 2;

std::array<std::array<int, 16>, 16> world_data = {{
    {{14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14}},
    {{14,  64,  14,  64,  14,  64,  64,  64,  64,  64,  64,  64,  64,  64,  64,  14}},
    {{14,  64,  14,  64,  64,  64,  14,  64,  64,  64,  64,  64,  64,  64,  64,  14}},
    {{14,  64,  14,  64,  64,  64,  14,  64,  64,  64,  13,  13,  64,  64,  12,  14}},
    {{14,  64,  14,  64,  64,  14,  64,  64,  64,  64,  13,  13,  13,  64,  12,  14}},
    {{14,  64,  14,  64,  64,  14,  64,  64,  64,  13,  13,  13,  13,  13,  12,  14}},
    {{14,  64,  14,  64,  64,  14,  64,  64,  64,  13,  13,  13,  13,  13,  12,  14}},
    {{14,  64,  57,  64,  64,  14,  64,  64,  64,  64,  13,  14,  14,  14,  12,  14}},
    {{14,  64,  57,  64,  64,  14,  64,  64,  64,  64,  14,  74,   8,   8,   8,  14}},
    {{14,  64,  14,  64,  64,  14,  64,  64,  64,  64,  14,  74,   8,   8,   8,  14}},
    {{14,  64,  14,  64,  64,  14,  64,  64,  64,  14,  57,   8,   8,   8,   8,  14}},
    {{14,  64,  14,  64,  64,  14,  64,  64,  74,  14,   8,   8,   8,   8,   8,  14}},
    {{14,  64,  14,  64,  64,  14,  64,  74,  74,  14,   8,   8,   8,   8,   8,  14}},
    {{14,  64,  14,  14,  14,  14,  64,  74,  74,  14,   8,   8,   8,   8,   8,  14}},
    {{14,  64,  14,   8,   8,   8,  64,  74,  74,  14,   8,   8,   8,   8,   8,  14}},
    {{14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14}}
}};

GLfloat * tileset_tex_coords;
GLfloat * map_data;
GLfloat * map_tex_coords;

struct Object {
    float x;
    float y;
} objects[num_objects];

void move_object(const int id, const float dx, const float dy) {
    if(id > num_objects || id < 0) {
        cerr << "ERROR: move_object: object id exceeds number of objects. Object id: " << id << endl;
        return;
    } 
    objects[id].x += dx;
    objects[id].y += dy;
}

static void init_buffers() {
    glUseProgram(program_obj);

    //vboIds[0] = map geometric data
    //vboIds[1] = tileset
    //vboIds[2] = sprite geometric data
    //vboIds[3] = texture coords for sprite
    glGenBuffers(num_vbo_ids, vboIds);

    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, map_height*map_width*sizeof(GLfloat)*18, map_data, GL_STATIC_DRAW);


    glBindBuffer(GL_ARRAY_BUFFER, vboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, map_height*map_width*sizeof(GLfloat)*12, map_tex_coords, GL_STATIC_DRAW);

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


    glVertexAttribPointer(VERTEX_POS_INDX, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glVertexAttribPointer(VERTEX_TEXCOORD0_INDX, 2, GL_FLOAT, GL_TRUE, 0, nullptr);

    glActiveTexture(GL_TEXTURE0);

    //Bind tiles texture
    glBindTexture(GL_TEXTURE_2D,texture_ids[0]);
}

static void animate(float dt) {
    // animate map
    float map_display_right_x = map_bottom_x + map_display_width;
    float map_display_top_y = map_bottom_y + map_display_height;

    // scroll the map towards the top right
    map_bottom_x += map_scroll_speed*dt;
    map_bottom_y += map_scroll_speed*dt;

    // perform wrapping
    if(map_display_right_x > map_width) {
        map_bottom_x = 0.0f;//wrap round
    }

    if(map_display_top_y > map_height) {
        map_bottom_y = 0.0f; //wrap round
    }


    // animate characters
    static float time_to_next = 0.0f;
    static int sequence[] = {1, 2, 3, 5, 7, 9, 11};
    static int i = 0;

    if(i == sizeof(sequence)/sizeof(int)) {
        i = 0;
    }

    time_to_next += dt;
    if(time_to_next > (1.0f/60.0f)) {
        time_to_next = 0.0f;
        
        int curr_tile = 0;// sequence[i++];
        GLfloat *tileset_ptr = &tileset_tex_coords[curr_tile*8];
        //bottom left
        sprite_tex_data[0] = tileset_ptr[0];
        sprite_tex_data[1] = tileset_ptr[1];
    
        //top left
        sprite_tex_data[2] = tileset_ptr[2];
        sprite_tex_data[3] = tileset_ptr[3];
    
        //bottom right
        sprite_tex_data[4] = tileset_ptr[4];
        sprite_tex_data[5] = tileset_ptr[5];
    
        //top left
        sprite_tex_data[6] = tileset_ptr[2];
        sprite_tex_data[7] = tileset_ptr[3];
    
        //top right
        sprite_tex_data[8] = tileset_ptr[6];
        sprite_tex_data[9] = tileset_ptr[7];
        
        //bottom right
        sprite_tex_data[10] = tileset_ptr[4];
        sprite_tex_data[11] = tileset_ptr[5];
        //changing texture coords
        glBindBuffer(GL_ARRAY_BUFFER, vboIds[3]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*12, sprite_tex_data, GL_DYNAMIC_DRAW);
    }
}

static void draw_map(int map_width, int map_height, float dt) {
    //LEAVE THIS HERE!
    glDisable(GL_CULL_FACE);
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 translated = glm::translate(model, glm::vec3(map_bottom_x, map_bottom_y, 0.0f));

    glUseProgram(program_obj);

    glUniformMatrix4fv(glGetUniformLocation(program_obj, "mat_projection"), 1, GL_FALSE,glm::value_ptr(projection_matrix));
    glUniformMatrix4fv(glGetUniformLocation(program_obj, "mat_modelview"), 1, GL_FALSE, glm::value_ptr(translated));

    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    glVertexAttribPointer(VERTEX_POS_INDX, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(VERTEX_POS_INDX);

    glBindBuffer(GL_ARRAY_BUFFER, vboIds[1]);
    glVertexAttribPointer(VERTEX_TEXCOORD0_INDX, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(VERTEX_TEXCOORD0_INDX);

    glBindAttribLocation(program_obj, VERTEX_POS_INDX, "a_position");

    glBindAttribLocation(program_obj, VERTEX_TEXCOORD0_INDX, "a_texCoord");

    glActiveTexture(GL_TEXTURE0);
   //Bind tiles texture
    glBindTexture(GL_TEXTURE_2D,texture_ids[0]);

   //set sampler texture to unit 0
    glUniform1i(glGetUniformLocation(program_obj, "s_texture"), 0);

    glDrawArrays(GL_TRIANGLES, 0, 6*map_width*map_height);
    glUseProgram(0);
}

static void draw_sprites(float dt) {
    glm::mat4 model = glm::mat4(1.0f);

    glUseProgram(program_obj);

    glUniformMatrix4fv(glGetUniformLocation(program_obj, "mat_projection"), 1, GL_FALSE,glm::value_ptr(projection_matrix));

    glBindBuffer(GL_ARRAY_BUFFER, vboIds[2]);
    glVertexAttribPointer(VERTEX_POS_INDX, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(VERTEX_POS_INDX);

    glBindBuffer(GL_ARRAY_BUFFER, vboIds[3]);
    glVertexAttribPointer(VERTEX_TEXCOORD0_INDX, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(VERTEX_TEXCOORD0_INDX);

    glBindAttribLocation(program_obj, VERTEX_POS_INDX, "a_position");

    glBindAttribLocation(program_obj, VERTEX_TEXCOORD0_INDX, "a_texCoord");

    glActiveTexture(GL_TEXTURE0);

    // Bind characters texture
    glBindTexture(GL_TEXTURE_2D,texture_ids[1]);

    // set sampler texture to unit 0
    glUniform1i(glGetUniformLocation(program_obj, "s_texture"), 0);

    for(int i = 0; i< num_objects; i++) {
        glm::mat4 translated = glm::translate(model, glm::vec3(objects[i].x, objects[i].y, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(program_obj, "mat_modelview"), 1, GL_FALSE, glm::value_ptr(translated));

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glUseProgram(0);
}

///
/// Initialise the opengl specifics for the game
///
static void init_ogl() {
    // Set background color and clear buffers
    glClearColor(0.15f, 0.25f, 0.35f, 1.0f);

    // Leave this here!!!
    // Disable back face culling.
    glDisable(GL_CULL_FACE);
}

///
/// Sets the OpenGL|ES model to default values
///
static void init_model_proj(GameWindow *window) {
    std::pair<int, int> size = window->get_size();
    glViewport(0, 0,  size.first, size.second);
    projection_matrix = glm::ortho(0.0f, (float)(size.first), 0.0f, (float)(size.second), -1.0f, 1.0f);
    //  glViewport(0, 0,  640, 480);
    //  projection_matrix = glm::ortho(0.0f, 640.0f, 0.0f, 480.0f, -1.0f, 1.0f);
}


///
/// Updates model projection to current position/rotation
///
static void update(float dt) {
    //   animate(dt);
}

///
/// Draws the model and calls eglSwapBuffers to render to screen
///
 static void redraw_scene(GameWindow *window, float dt)
 {
   glClear( GL_COLOR_BUFFER_BIT );

   draw_map(16, 16, dt);
   draw_sprites( dt);
   window->swap_buffers();
 }


static void generate_tileset_coords(int image_width, int image_height) {
#ifdef DEBUG
    printf("GENERATING TILESET TEXTURE COORDS...");
#endif
    //check the tilset image height and widths are multiples of the tiles
    //  assert(image_height % TILESET_ELEMENT_SIZE != 0 || image_width % TILESET_ELEMENT_SIZE != 0);

    assert(TILESET_ELEMENT_SIZE);

    int num_tiles_x = image_width  / TILESET_ELEMENT_SIZE;
    int num_tiles_y = image_height / TILESET_ELEMENT_SIZE;
    std::cout << num_tiles_x << " " << num_tiles_y << endl;

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


///
///  Name: generate_map_texcoords
///  Description: This code generates the texture coordinate
///      that we use for the map
/// 
///
static void generate_map_texcoords(int map_width, int map_height) {
#ifdef DEBUG
    printf("GENERATING MAP TEXTURE DATA...");
#endif
    //holds the map data
    //need 12 float for the 2D texture coordinates
    int num_floats = 12;
    map_tex_coords = new GLfloat[sizeof(GLfloat)*map_height*map_width*num_floats]; 
    assert(map_tex_coords);

    int x, y;

    //generate the map data
    // get the tile set coordinates for the particular tile
    for(x = 0; x < map_width; x++) {
        for(y = 0; y < map_height; y++) {
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
}

/***********************************************************
 * Name: generate_map_coords
 * Description: This code generates the vertex data for the map
 *
 ***********************************************************/
static void generate_map_coords(int map_width, int map_height) {
#ifdef DEBUG
    printf("GENERATING MAP DATA...");
#endif
    //holds the map data
    //need 18 floats for each coordinate as these hold 3D coordinates
    int num_floats = 18;
    map_data = new GLfloat[sizeof(GLfloat)*map_height*map_width*num_floats]; 
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
    int x, y;
    for(x = 0; x < map_width; x++) {
        for(y = 0; y < map_height; y++) {
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
}

GLuint load_shader(GLenum type, const std::string src) {
    GLuint shader;
    GLint compiled = 0;
    
    // Create the shader object 
    shader = glCreateShader(type);

    if(shader == 0) {
        // Couldn't create the shader
        return 0;
    }

    // Load shader source code
    const char* source = src.c_str();
    glShaderSource(shader, 1, &source, nullptr);

    // Compile the shader
    glCompileShader(shader);

    // Check for errors
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    // Handle the errors
    if(!compiled) {
        GLint info_len = 0;
      
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);
      
        if(info_len > 1) {
            char* info_log = new char[sizeof(char) * info_len];

            glGetShaderInfoLog(shader, info_len, nullptr, info_log);
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
        
            glGetProgramInfoLog(program_obj, info_len, nullptr, info_log);
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

/// Much easier solution to reading files:
/// http://stackoverflow.com/a/2602060/1763356
static void load_tex_images() {
    FILE *tex_file1, *tex_file2 = nullptr;
    size_t bytes_read = 0;
    size_t image_sz_1 = IMAGE1_SIZE_WIDTH*IMAGE1_SIZE_HEIGHT*IMAGE1_NUM_COMPONENTS;
    size_t image_sz_2 = IMAGE2_SIZE_WIDTH*IMAGE2_SIZE_HEIGHT*IMAGE2_NUM_COMPONENTS;
    tex_buf1 = new char[image_sz_1];

    tex_file1 = fopen(PATH "../resources/basictiles_2.raw", "rb");
    if(tex_file1 == nullptr) {
        std::cerr << "ERROR: Couldn't load textures" << endl;
    }

    if (tex_file1 && tex_buf1) {
        bytes_read = fread(tex_buf1, 1, image_sz_1, tex_file1);
        assert(bytes_read == image_sz_1);  // some problem with file?
        fclose(tex_file1);
    }

    tex_buf2 = new char[image_sz_2];

    tex_file2 = fopen(PATH "../resources/characters_1.raw", "rb");
    if(tex_file2 == nullptr) {
        std::cerr << "ERROR: Couldn't load textures" << endl;
    }

    if (tex_file2 && tex_buf2) {
        bytes_read = fread(tex_buf2, 1, image_sz_2, tex_file2);
        assert(bytes_read == image_sz_2);  // some problem with file?
        fclose(tex_file2);
    }
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
static void init_textures() {
    //  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    load_tex_images();
    glGenTextures(2, texture_ids);
    glBindTexture(GL_TEXTURE_2D, texture_ids[0]);
  
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, IMAGE1_SIZE_WIDTH, IMAGE1_SIZE_HEIGHT, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, tex_buf1);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, texture_ids[1]);
  
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, IMAGE2_SIZE_WIDTH, IMAGE2_SIZE_HEIGHT, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, tex_buf2);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST);
}


//------------------------------------------------------------------------------

// Function to be passed to atexit().
static void exit_func(void) {
    // clear screen
    glClear( GL_COLOR_BUFFER_BIT );

    // release buffers
    delete[] tex_buf1;
    delete[] tex_buf2;
    delete[] map_data;
    delete[] sprite_data;
    delete[] sprite_tex_data;
    delete[] map_tex_coords;
    delete[] tileset_tex_coords;

    printf("\nClosed\n");
}


static float get_dt() {
    static std::chrono::steady_clock::time_point curr_time = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point milliseconds = std::chrono::steady_clock::now();   

    typedef std::chrono::duration<int, std::milli> millisecs_t;
    millisecs_t duration(std::chrono::duration_cast<millisecs_t>(milliseconds - curr_time));

    curr_time = milliseconds;
    return static_cast<float>(duration.count()) / 1000.0f;
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

  if (!vertex_shader_src.good()){
    std::cerr << "Failed to load vertex shader" << endl;
    return false;
  }
    
    if (!fragment_shader_src.good()) {
        cerr << "Failed to load fragment shader" << endl;
        return false;
    }

    std::string vert_src, frag_src, line;
    while (getline(vertex_shader_src, line)) {
        vert_src += line + "\n";
    }

    while (getline(fragment_shader_src, line)) {
        frag_src += line + "\n";
    }

    GLuint program_obj = shader_create(vert_src, frag_src);

    if (program_obj == 0) {
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
    assert(sprite_tex_data);

    //generate the map data

    int curr_tile = 0;
    GLfloat *tileset_ptr = &tileset_tex_coords[curr_tile*8];
    GLfloat offset_x = GLfloat(1.0 / (IMAGE2_SIZE_WIDTH  / TILESET_ELEMENT_SIZE));
    GLfloat offset_y = GLfloat(1.0 / (IMAGE2_SIZE_HEIGHT / TILESET_ELEMENT_SIZE));

   //bottom left
    sprite_tex_data[0] = offset_x; // tileset_ptr[0];
    sprite_tex_data[1] = offset_y; //tileset_ptr[1];

    //top left
    sprite_tex_data[2] = offset_x; //tileset_ptr[2];
    sprite_tex_data[3] = 0.0f; //tileset_ptr[3];

    //bottom right
    sprite_tex_data[4] = offset_x*2.0f;//tileset_ptr[4];
    sprite_tex_data[5] = offset_y; //tileset_ptr[5];

    //top left
    sprite_tex_data[6] = offset_x;//tileset_ptr[2];
    sprite_tex_data[7] = 0.0f;//tileset_ptr[3];

    //top right
    sprite_tex_data[8] = offset_x*2.0f;//tileset_ptr[6];
    sprite_tex_data[9] = 0.0f;//tileset_ptr[7];

    //bottom right
    sprite_tex_data[10] = offset_x*2.0f;//tileset_ptr[4];
    sprite_tex_data[11] = offset_y;//tileset_ptr[5];

    for (int i =0 ; i< 12; i++) {
        std::cout << " " << sprite_tex_data[i];
    }
    std::cout << std::endl;
}

void generate_sprite_coords() {
#ifdef DEBUG
    printf("GENERATING MAP DATA...");
#endif
    //holds the map data
    //need 18 floats for each coordinate as these hold 3D coordinates
    int num_floats = 18;
    sprite_data  = new GLfloat[sizeof(GLfloat)*num_floats]; 
    assert(sprite_data);
    float scale = TILESET_ELEMENT_SIZE * GLOBAL_SCALE;
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



int main (int argc, char* argv[]) {
  bool use_graphical_window = true;

  //Determine if the no-window command was sent
  if(argc == 2) {
    std::string param = argv[1];

    if(param == "no-window")
      use_graphical_window = false;
  }
  //TODO: Support no window


  GameWindow window(map_width*TILESET_ELEMENT_SIZE*GLOBAL_SCALE, map_height*TILESET_ELEMENT_SIZE*GLOBAL_SCALE, false);
  window.use_context();

  Map map("");
  MapViewer map_viewer(&window);
  map_viewer.set_map(&map);
  std::thread mythread(run_all);

  float dt = get_dt();
  int count = 0;

  while (!window.check_close()) {
    //    init_model_proj(&window);

    //Get the time since the last iteration 
    dt = get_dt(); 
    map_viewer.update_map(dt);
    map_viewer.render_map();
    //    update(dt);
    //    redraw_scene(&window, dt);
    GameWindow::update();
  }

  //  exit_func();

  //
  // Typically one would do
  //     mythread.join();
  // but this hangs due to the extra, unkilled threads.
  //
  // As a hack, just let it crash.
  //

  return 1;
}

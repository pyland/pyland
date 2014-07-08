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

using namespace std;    


#define GLOBAL_SCALE 2
static volatile int shutdown;
const int num_objects = 2;

struct Object {
    float x;
    float y;
} objects[num_objects];

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


void move_object(const int id, const float dx, const float dy) {
    if(id > num_objects || id < 0) {
        cerr << "ERROR: move_object: object id exceeds number of objects. Object id: " << id << endl;
        return;
    } 
    objects[id].x += dx;
    objects[id].y += dy;
}

/*
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
*/
/*
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
*/


static float get_dt() {
    static std::chrono::steady_clock::time_point curr_time = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point milliseconds = std::chrono::steady_clock::now();   

    typedef std::chrono::duration<int, std::milli> millisecs_t;
    millisecs_t duration(std::chrono::duration_cast<millisecs_t>(milliseconds - curr_time));

    curr_time = milliseconds;
    return static_cast<float>(duration.count()) / 1000.0f;
}
/*


*/
int main (int argc, char* argv[]) {
  bool use_graphical_window = true;

  //Determine if the no-window command was sent
  if(argc == 2) {
    std::string param = argv[1];

    if(param == "no-window")
      use_graphical_window = false;
  }
  //TODO: Support no window
  //Can't do this cleanly at the moment as the MapViewer needs the window instance.... 
  int map_width = 16, map_height = 16;
  GameWindow window(map_width*TILESET_ELEMENT_SIZE*GLOBAL_SCALE, map_height*TILESET_ELEMENT_SIZE*GLOBAL_SCALE, false);
  window.use_context();

  Map map("");
  MapViewer map_viewer(&window);
  map_viewer.set_map(&map);
  std::thread mythread(run_all);

  float dt = get_dt();
  int count = 0;

  while (!window.check_close()) {
    //Get the time since the last iteration 
    dt = get_dt(); 
    map_viewer.update_map(dt);
    map_viewer.render_map();

    GameWindow::update();
  }

  //
  // Typically one would do
  //     mythread.join();
  // but this hangs due to the extra, unkilled threads.
  //
  // As a hack, just let it crash.
  //

  return 1;
}

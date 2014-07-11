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

#include <boost/filesystem.hpp>
    
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <sys/time.h>
#include <thread>
#include <unistd.h>

//Include GLM
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "character.hpp"
#include "engine_api.hpp"
#include "game_window.hpp"
#include "input_manager.hpp"
#include "interpreter.hpp"
#include "keyboard_input_event.hpp"
#include "lifeline.hpp"
#include "main.hpp"
#include "map.hpp"
#include "map_viewer.hpp"
#include "print_debug.hpp"

#ifdef USE_GLES

#include <GLES2/gl2.h>

#endif

#ifdef USE_GL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif



using namespace std;    


#define GLOBAL_SCALE 2
static volatile int shutdown;

static std::mt19937 random_generator;


//TODO: Move this out of here, just added to get the sprite python control working again before full
//object management is done
std::map<int, Character*>* characters;

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
    int new_id = id +1;

    characters->at(new_id)->set_x_position(characters->at(new_id)->get_x_position() + dx);
    characters->at(new_id)->set_y_position(characters->at(new_id)->get_y_position() + dy);
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

static float get_dt() {
    static std::chrono::steady_clock::time_point curr_time = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point milliseconds = std::chrono::steady_clock::now();   

    typedef std::chrono::duration<int, std::milli> millisecs_t;
    millisecs_t duration(std::chrono::duration_cast<millisecs_t>(milliseconds - curr_time));

    curr_time = milliseconds;
    return static_cast<float>(duration.count()) / 1000.0f;
}


static int maxid = 0;
void create_character(Interpreter &interpreter, std::map<int, Character *> *characters, int x, int y, std::string name) {
    ++maxid;

    print_debug << "Creating character" << std::endl;

    Character* new_character = new Character();
    new_character->set_id(maxid);
    new_character->set_name("John");

    print_debug << "Adding character" << std::endl;

    (*characters)[maxid] = new_character;

    print_debug << "Creating character wrapper" << std::endl;

    std::uniform_int_distribution<int32_t> random_x(1, 14);
    std::uniform_int_distribution<int32_t> random_y(1, 14);
    Entity *a_thing = new Entity(Vec2D(random_x(random_generator), random_y(random_generator)), name, maxid-1);

    print_debug << "Registering character" << std::endl;

    interpreter.register_entity(*a_thing);

    print_debug << "Done!" << std::endl;
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
    //Can't do this cleanly at the moment as the MapViewer needs the window instance.... 
    int map_width = 16, map_height = 16;
    GameWindow window(map_width*TILESET_ELEMENT_SIZE*GLOBAL_SCALE, map_height*TILESET_ELEMENT_SIZE*GLOBAL_SCALE, false);
    window.use_context();

    Map map("");

    characters = map.get_characters_map();


    Interpreter interpreter(boost::filesystem::absolute("python_embed/wrapper_functions.so").normalize());

    MapViewer map_viewer(&window);
    map_viewer.set_map(&map);


    float dt = get_dt();
    int count = 0;


    InputManager* input_manager = window.get_input_manager();

    Lifeline callback_lifeline = input_manager->register_keyboard_handler(
        [] (KeyboardInputEvent event) {
            print_debug
                << "Keyboard Event:\tKeycode:\t" << event.key_code 
                << "\tScancode:\t" << event.scan_code 
                << "\tDown:\t" << event.down 
                << "\tChanged:\t" << event.changed
                << "\tRepeated:\t" << event.repeated
                << std::endl;
    });

    Lifeline press_lifeline = input_manager->register_key_press_handler(
        [&] (KeyboardInputEvent event) {
            print_debug
                << "Key Press:\tKeycode:\t" << event.key_code 
                << "\tScancode:\t" << event.scan_code 
                << std::endl;

            create_character(interpreter, characters, maxid, maxid, "Adam");
    });

    Lifeline type_lifeline = input_manager->register_key_type_handler(
        [] (KeyboardInputEvent event) {
            print_debug
                << "Key Typed:\tKeycode:\t" << event.key_code 
                << "\tScancode:\t" << event.scan_code 
                << std::endl;
    });

    Lifeline release_lifeline = input_manager->register_key_release_handler(
        [] (KeyboardInputEvent event) {
            print_debug
                << "Key Release:\tKeycode:\t" << event.key_code 
                << "\tScancode:\t" << event.scan_code 
                << std::endl;
    });
    
    Lifeline down_lifeline = input_manager->register_key_down_handler(
        [] (KeyboardInputEvent event) {
            print_debug
                << "Key Down:\tKeycode:\t" << event.key_code 
                << "\tScancode:\t" << event.scan_code 
                << std::endl;
    });


    while (!window.check_close()) {
        //Get the time since the last iteration 
        dt = get_dt(); 
        map_viewer.update_map(dt);
        map_viewer.render_map();

        GameWindow::update();
    }

    return 0;
}

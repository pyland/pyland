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
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <map>
#include <memory>
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

#include "api.hpp"
#include "character.hpp"
#include "engine_api.hpp"
#include "event_manager.hpp"
#include "filters.hpp"
#include "game_window.hpp"
#include "input_manager.hpp"
#include "interpreter.hpp"
#include "keyboard_input_event.hpp"
#include "lifeline.hpp"
#include "locks.hpp"
#include "main.hpp"
#include "make_unique.hpp"
#include "map.hpp"
#include "map_viewer.hpp"
#include "object_manager.hpp"
#include "typeface.hpp"
#include "text_font.hpp"
#include "text.hpp"

// Include challenges
// TODO: Rearchitechture
#include "challenge.hpp"
#include "long_walk_challenge.hpp"

// Choose between GLES and GL
#ifdef USE_GLES
#include <GLES2/gl2.h>
#endif

#ifdef USE_GL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif



using namespace std;    

enum arrow_key {UP, DOWN, LEFT, RIGHT};

#define GLOBAL_SCALE 1
static volatile int shutdown;

static std::mt19937 random_generator;

void create_character(Interpreter &interpreter) {
    LOG(INFO) << "Creating character";

    // Registering new character with game engine
    shared_ptr<Character> new_character = make_shared<Character>();
    new_character->set_name("John");

    int start_x = 4;
    int start_y = 15;
    new_character->set_x_position(start_x);
    new_character->set_y_position(start_y);
    LOG(INFO) << "Adding character";
    ObjectManager::get_instance().add_object(new_character);
    Engine::get_map_viewer()->get_map()->add_character(new_character->get_id());
    
    Engine::get_map_viewer()->set_map_focus_object(new_character->get_id());
    LOG(INFO) << "Creating character wrapper";
    LOG(INFO) << "ID " << new_character->get_id();

    // Register user controled character
    // Yes, this is a memory leak. Deal with it.
    Entity *a_thing = new Entity(Vec2D(start_x, start_y), new_character->get_name(), new_character->get_id());

    LOG(INFO) << "Registering character";

    new_character->daemon = std::make_unique<LockableEntityThread>(interpreter.register_entity(*a_thing));

    LOG(INFO) << "Done!";
}

class CallbackState {
    public:
        CallbackState(Interpreter &interpreter,
                      std::string name):
            interpreter(interpreter),
            name(name){
        }

        void register_number(int id) {
            LOG(INFO) << "changing focus to " << id;
            Engine::get_map_viewer()->set_map_focus_object(id);
        }

        void spawn() {
            create_character(interpreter);
        }

        void restart() {
            auto id = Engine::get_map_viewer()->get_map_focus_object();
            auto active_player = ObjectManager::get_instance().get_object<Object>(id);

            if (!active_player) { return; }

            active_player->daemon->value->halt_soft(EntityThread::Signal::RESTART);
        }

        void stop() {

            auto id = Engine::get_map_viewer()->get_map_focus_object();
            auto active_player = ObjectManager::get_instance().get_object<Object>(id);

            if (!active_player) { return; }

            active_player->daemon->value->halt_soft(EntityThread::Signal::STOP);
        }

        void kill() {

            auto id = Engine::get_map_viewer()->get_map_focus_object();
            auto active_player = ObjectManager::get_instance().get_object<Object>(id);

            if (!active_player) { return; }

            active_player->daemon->value->halt_soft(EntityThread::Signal::KILL);
        }

        void man_move (arrow_key direction) {
            VLOG(3) << "arrow key pressed";
            auto id = Engine::get_map_viewer()->get_map_focus_object();
            switch (direction) {
                case (UP):
                    Engine::move_object(id,Vec2D(0,1));
                    break;
                case (DOWN):
                    Engine::move_object(id,Vec2D(0,-1));
                    break;
                case (RIGHT):
                    Engine::move_object(id,Vec2D(-1,0));
                    break;
                case (LEFT):
                    Engine::move_object(id,Vec2D(1,0));
                    break;
            }

        }

        void monologue () {
            auto id = Engine::get_map_viewer()->get_map_focus_object();
            Vec2D location =  Engine::find_object(id);
            std::cout << "You are at " << location.to_string() <<std::endl;
        }

    private:
        Interpreter &interpreter;
        std::string name;
};


int main(int argc, const char* argv[]) {
    //    bool use_graphical_window = true;

    //Determine if the no-window command was sent
    /*    if (argc == 2) {
        std::string param = argv[1];

        if (param == "no-window") {
            use_graphical_window = false;
        }
    }
    */
    // TODO: Support no window
    // Can't do this cleanly at the moment as the MapViewer needs the window instance.... 

    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();

    int map_width = 16, map_height = 16;
    GameWindow window(map_width*TILESET_ELEMENT_SIZE*GLOBAL_SCALE, map_height*TILESET_ELEMENT_SIZE*GLOBAL_SCALE, false);
    window.use_context();

    Map map("../resources/map0.tmx");

    Interpreter interpreter(boost::filesystem::absolute("python_embed/wrapper_functions.so").normalize());

    MapViewer map_viewer(&window);
    map_viewer.set_map(&map);

    Engine::set_map_viewer(&map_viewer);

    CallbackState callbackstate(interpreter, "John");

    InputManager* input_manager = window.get_input_manager();

    Lifeline stop_callback = input_manager->register_keyboard_handler(filter(
        {KEY_PRESS, KEY("H")},
        [&] (KeyboardInputEvent) { callbackstate.stop(); }
    ));
    Lifeline restart_callback = input_manager->register_keyboard_handler(filter(
        {KEY_PRESS, KEY("R")},
        [&] (KeyboardInputEvent) { callbackstate.restart(); }
    ));

    Lifeline up_callback = input_manager->register_keyboard_handler(filter(
        {ANY_OF({KEY_HELD}), KEY({"Up", "W"})},
        [&] (KeyboardInputEvent) { callbackstate.man_move(UP); }
    ));

    Lifeline down_callback = input_manager->register_keyboard_handler(filter(
        {ANY_OF({KEY_HELD}), KEY({"Down","S"})},
        [&] (KeyboardInputEvent) { callbackstate.man_move(DOWN); }
    ));

    Lifeline right_callback = input_manager->register_keyboard_handler(filter(
        {ANY_OF({KEY_HELD}), KEY({"Right","D"})},
        [&] (KeyboardInputEvent) { callbackstate.man_move(LEFT); }
    ));

    Lifeline left_callback = input_manager->register_keyboard_handler(filter(
        {ANY_OF({KEY_HELD}), KEY({"Left","A"})},
        [&] (KeyboardInputEvent) { callbackstate.man_move(RIGHT); }
    ));

    Lifeline monologue_callback = input_manager->register_keyboard_handler(filter(
        {ANY_OF({KEY_PRESS}), KEY("M")},
        [&] (KeyboardInputEvent) { callbackstate.monologue(); }
    ));

    std::vector<Lifeline> digit_callbacks;
    for (int i=0; i<10; ++i) {
        digit_callbacks.push_back(
            input_manager->register_keyboard_handler(filter(
                {KEY_PRESS, KEY(std::to_string(i))},
                [&, i] (KeyboardInputEvent) { callbackstate.register_number(i); }
            ))
        );
    }

    EventManager &em = EventManager::get_instance();

    Typeface mytype("../fonts/hans-kendrick/HansKendrick-Regular.ttf");
    TextFont myfont(mytype, 18);
    Text mytext(&window, myfont, true);
    mytext.set_text("John");
    mytext.move(100, 100);
    mytext.resize(800, 80);

    std::string editor;

    if (argc >= 2) {
        Engine::set_editor(argv[1]);
    };

    LongWalkChallenge long_walk_challenge(&mytext);
    callbackstate.spawn();
    long_walk_challenge.start();

    while (!window.check_close()) {
        //Get the time since the last iteration 
        em.process_events();
        map_viewer.render_map();
        mytext.display();
        window.swap_buffers();
        GameWindow::update();
    }

    return 0;
}
// 

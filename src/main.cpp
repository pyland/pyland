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
#include "print_debug.hpp"
#include "challenge1.hpp"

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

static float get_dt() {
    static std::chrono::steady_clock::time_point curr_time = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point milliseconds = std::chrono::steady_clock::now();   

    typedef std::chrono::duration<int, std::milli> millisecs_t;
    millisecs_t duration(std::chrono::duration_cast<millisecs_t>(milliseconds - curr_time));

    curr_time = milliseconds;
    return static_cast<float>(duration.count()) / 1000.0f;
}

void create_character(Interpreter &interpreter) {

    print_debug << "Creating character" << std::endl;

    // Registering new character with game engine
    shared_ptr<Character> new_character = make_shared<Character>();
    new_character->set_name("John");

    int start_x = 4;
    int start_y = 15;
    new_character->set_x_position(start_x);
    new_character->set_y_position(start_y);
    print_debug << "Adding character" << std::endl;
    ObjectManager::get_instance().add_object(new_character);
    Engine::get_map_viewer()->get_map()->add_character(new_character->get_id());
    
    Engine::get_map_viewer()->set_map_focus_object(new_character->get_id());
    print_debug << "Creating character wrapper" << std::endl;
    std::cout<< "ID " << new_character->get_id() <<std::endl;

    // Register user controled character
    // Yes, this is a memory leak. Deal with it.
    Entity *a_thing = new Entity(Vec2D(start_x, start_y), new_character->get_name(), new_character->get_id());

    print_debug << "Registering character" << std::endl;

    new_character->daemon = std::make_unique<LockableEntityThread>(interpreter.register_entity(*a_thing));

    print_debug << "Done!" << std::endl;
}

class CallbackState {
    public:
        CallbackState(Interpreter &interpreter,
                      std::string name):
            interpreter(interpreter),
            name(name){
        }

        void register_number(int id) {
            print_debug << "changing focus to " << id << std::endl;
            Engine::get_map_viewer()->set_map_focus_object(id);
        }

        void spawn() {
            create_character(interpreter);
        }

        void restart(InterpreterContext interpreter_context) {
            auto id = Engine::get_map_viewer()->get_map_focus_object();
            auto active_player = ObjectManager::get_instance().get_object<Object>(id);

            if (!active_player) { return; }

            lock::GIL lock_gil(interpreter_context, "Killer");

            // TODO: Lock
            active_player->daemon->value->halt_soft(EntityThread::Signal::RESTART);
        }

        void stop(InterpreterContext interpreter_context) {

            auto id = Engine::get_map_viewer()->get_map_focus_object();
            auto active_player = ObjectManager::get_instance().get_object<Object>(id);

            if (!active_player) { return; }

            lock::GIL lock_gil(interpreter_context, "Killer");

            // TODO: Lock
            active_player->daemon->value->halt_soft(EntityThread::Signal::STOP);
        }

        void kill(InterpreterContext interpreter_context) {

            auto id = Engine::get_map_viewer()->get_map_focus_object();
            auto active_player = ObjectManager::get_instance().get_object<Object>(id);

            if (!active_player) { return; }

            lock::GIL lock_gil(interpreter_context, "Killer");

            // TODO: Lock
            active_player->daemon->value->halt_soft(EntityThread::Signal::KILL);
        }

        void man_move (arrow_key direction) {
            print_debug << "arrow key pressed " << std::endl;
            auto id = Engine::get_map_viewer()->get_map_focus_object();
            switch (direction) {
                case (UP):
                    Engine::move_object(id,0,1);
                    break;
                case (DOWN):
                    Engine::move_object(id,0,-1);
                    break;
                case (RIGHT):
                    Engine::move_object(id,-1,0);
                    break;
                case (LEFT):
                    Engine::move_object(id,1,0);
                    break;
            }

        }

        void monologue () {
            auto id = Engine::get_map_viewer()->get_map_focus_object();
            std::pair<int,int> location =  Engine::find_object(id);
            std::cout << "You are at (" << location.first << "," << location.second << ")" <<std::endl;

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
    int map_width = 16, map_height = 16;
    GameWindow window(map_width*TILESET_ELEMENT_SIZE*GLOBAL_SCALE, map_height*TILESET_ELEMENT_SIZE*GLOBAL_SCALE, false);
    window.use_context();

    Map map("../resources/map0.tmx");

    Interpreter interpreter(boost::filesystem::absolute("python_embed/wrapper_functions.so").normalize());

    MapViewer map_viewer(&window);
    map_viewer.set_map(&map);

    Engine::set_map_viewer(&map_viewer);
    
    float dt = get_dt();

    CallbackState callbackstate(interpreter, "John");

    InputManager* input_manager = window.get_input_manager();

    Lifeline spawn_callback = input_manager->register_keyboard_handler(filter(
        {KEY_PRESS, KEY("N")},
        [&] (KeyboardInputEvent) { callbackstate.spawn(); }
    ));

    Lifeline kill_callback = input_manager->register_keyboard_handler(filter(
        {KEY_PRESS, KEY("K")},
        [&] (KeyboardInputEvent) { callbackstate.kill(interpreter.interpreter_context); }
    ));
    Lifeline stop_callback = input_manager->register_keyboard_handler(filter(
        {KEY_PRESS, KEY("H")},
        [&] (KeyboardInputEvent) { callbackstate.stop(interpreter.interpreter_context); }
    ));
    Lifeline restart_callback = input_manager->register_keyboard_handler(filter(
        {KEY_PRESS, KEY("R")},
        [&] (KeyboardInputEvent) { callbackstate.restart(interpreter.interpreter_context); }
    ));

    Lifeline up_callback = input_manager->register_keyboard_handler(filter(
        {ANY_OF({KEY_REPEAT,KEY_PRESS}), KEY({"Up", "W"})},
        [&] (KeyboardInputEvent) { callbackstate.man_move(UP); }
    ));

    Lifeline down_callback = input_manager->register_keyboard_handler(filter(
        {ANY_OF({KEY_REPEAT,KEY_PRESS}), KEY({"Down","S"})},
        [&] (KeyboardInputEvent) { callbackstate.man_move(DOWN); }
    ));

    Lifeline right_callback = input_manager->register_keyboard_handler(filter(
        {ANY_OF({KEY_REPEAT,KEY_PRESS}), KEY({"Right","D"})},
        [&] (KeyboardInputEvent) { callbackstate.man_move(LEFT); }
    ));

    Lifeline left_callback = input_manager->register_keyboard_handler(filter(
        {ANY_OF({KEY_REPEAT,KEY_PRESS}), KEY({"Left","A"})},
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

    ///////////////////////////////

    EventManager& em = EventManager::get_instance();
    auto func1 = [] () { std::cout << "FUNC 1" << std::endl;};
    auto func2 = [] () { std::cout << "FUNC 2" << std::endl; };
    auto func3 = [] () { std::cout << "FUNC 3" << std::endl; };
    auto func4 = [] () { std::cout << "FUNC 4" << std::endl; };
    auto func_t1 = [] (double percent) -> bool { std::cout << "TIMER" << std::endl; if(percent >= 1.0) return false; else return true;  };

    em.add_event(func1);

    em.add_event(func2);
    em.add_event(func3);
    em.add_timed_event(std::chrono::duration<double>(0.5), func_t1);

    em.process_events();

    em.add_event(func3);

    em.add_event(func4);

    ///////////////////////////////

    std::string editor;

    if (argc == 2) {
        editor = argv[1];
    } else {
        editor = "gedit";
    };

    Chal1 challenge1 = Chal1(editor);

    callbackstate.spawn();

    while (!window.check_close()) {
        //Get the time since the last iteration 
        dt = get_dt(); 
        em.process_events();
        map_viewer.update_map(dt);
        map_viewer.render_map();

        GameWindow::update();
    }

    return 0;
}
// 

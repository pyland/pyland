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
n
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
#include "button.hpp"
#include "character.hpp"
#include "component.hpp"
#include "engine_api.hpp"
#include "event_manager.hpp"
#include "filters.hpp"
#include "game_window.hpp"
#include "gui_manager.hpp"
#include "gui_window.hpp"
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
#define TEXT_BORDER_WIDTH 20
#define TEXT_HEIGHT 80

static volatile int shutdown;

static std::mt19937 random_generator;

int create_character(Interpreter &interpreter) {
    LOG(INFO) << "Creating character";


    int start_x = 4;
    int start_y = 15;

    // Registering new character with game engine
    shared_ptr<Character> new_character = make_shared<Character>(start_x, start_y);
    new_character->set_name("John");
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

    return new_character->get_id();
}

class CallbackState {
    public:
        CallbackState(Interpreter &interpreter,
                      std::string name):
            interpreter(interpreter),
            name(name){
        }

        void register_number(int key) {
            LOG(INFO) << "changing focus to " << key;

            if (!(0 < key && size_t(key) < key_to_id.size())) {
                LOG(INFO) << "changing focus aborted; no such id";
                return;
            }

            Engine::get_map_viewer()->set_map_focus_object(key_to_id[key]);
        }

        void spawn() {
            key_to_id.push_back(create_character(interpreter));
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
        std::vector<int> key_to_id;
};


int main(int argc, const char* argv[]) {
    // TODO: Support no window
    // Can't do this cleanly at the moment as the MapViewer needs the window instance.... 

    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();

    int map_width = 32, map_height = 32;
    GameWindow window(map_width*TILESET_ELEMENT_SIZE*GLOBAL_SCALE, map_height*TILESET_ELEMENT_SIZE*GLOBAL_SCALE, false);
    window.use_context();

    Map map("../resources/map0.tmx");

    Interpreter interpreter(boost::filesystem::absolute("python_embed/wrapper_functions.so").normalize());

    //BUILD the GUI
    GUIManager gui_manager;

    void (GUIManager::*mouse_callback_function) (MouseInputEvent) = &GUIManager::mouse_callback_function;

    //TODO : REMOVE THIS HACKY EDIT - done for the demo tomorrow
    Typeface buttontype("../fonts/hans-kendrick/HansKendrick-Regular.ttf");
    TextFont buttonfont(buttontype, 18);
    Text stoptext(&window, buttonfont, true);
    Text runtext(&window, buttonfont, true);
    stoptext.set_text("Stop");
    runtext.set_text("Run");
    // referring to top left corner of text window
    stoptext.move(105, 240 + 20);
    runtext.move(5, 240 + 20);
    stoptext.resize(window.get_size().first-20, 80 + 20);
    runtext.resize(window.get_size().first-20, 80 + 20);


    CallbackState callbackstate(interpreter, "John");


    std::shared_ptr<GUIWindow> sprite_window = std::make_shared<GUIWindow>();;
    sprite_window->set_width_pixels(300);
    sprite_window->set_height_pixels(300);
    std::shared_ptr<Button> run_button = std::make_shared<Button>();
    run_button->set_text("Run");
    run_button->set_on_click([&] () { LOG(ERROR) << "RUN"; callbackstate.restart(); });
    run_button->set_width(0.2f);
    run_button->set_height(0.2f);
    run_button->set_y_offset(0.8f);
    run_button->set_x_offset(0.0f);

    std::shared_ptr<Button> stop_button = std::make_shared<Button>();
    stop_button->set_text("Stop");
    stop_button->set_on_click([&] () {LOG(ERROR) << "STOP";  callbackstate.stop(); });
    stop_button->set_width(0.2f);
    stop_button->set_height(0.2f);
    stop_button->set_y_offset(0.8f);
    stop_button->set_x_offset(0.8f);

    sprite_window->add(run_button);
    sprite_window->add(stop_button);

    gui_manager.set_root(sprite_window);

    // quick fix so buttons in correct location in initial window before gui_resize_func callback
    auto original_window_size = window.get_size();
    sprite_window->set_width_pixels(original_window_size.first);
    sprite_window->set_height_pixels(original_window_size.second);

    gui_manager.parse_components();

    std::function<void(GameWindow*)> gui_resize_func = [&] (GameWindow* game_window) { 
        LOG(INFO) << "GUI resizing"; 
        auto window_size = (*game_window).get_size();
        sprite_window->set_width_pixels(window_size.first);
        sprite_window->set_height_pixels(window_size.second);
        gui_manager.parse_components();
    };
    Lifeline gui_resize_lifeline = window.register_resize_handler(gui_resize_func);


    MapViewer map_viewer(&window,&gui_manager);
    map_viewer.set_map(&map);


    Engine::set_map_viewer(&map_viewer);

    InputManager* input_manager = window.get_input_manager();

    Lifeline stop_callback = input_manager->register_keyboard_handler(filter(
        {KEY_PRESS, KEY("H")},
        [&] (KeyboardInputEvent) { callbackstate.stop(); }
    ));
    Lifeline spawn_callback = input_manager->register_keyboard_handler(filter(
        {KEY_PRESS, KEY("N")},
        [&] (KeyboardInputEvent) { callbackstate.spawn(); }
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

    Lifeline mouse_button_lifeline = input_manager->register_mouse_handler(filter({ANY_OF({ MOUSE_RELEASE})}, [&] (MouseInputEvent event) {(gui_manager.*mouse_callback_function) (event);}));


    std::vector<Lifeline> digit_callbacks;
    for (int i=0; i<10; ++i) {
        digit_callbacks.push_back(
            input_manager->register_keyboard_handler(filter(
                {KEY_PRESS, KEY(std::to_string(i))},
                [&, i] (KeyboardInputEvent) { callbackstate.register_number(i); }
            ))
        );
    }

    Lifeline switch_char = input_manager->register_mouse_handler(filter({ANY_OF({ MOUSE_RELEASE})}, 
        [&] (MouseInputEvent event) {
            LOG(INFO) << "mouse clicked on map at " << event.to.x << " " << event.to.y << " pixel";
            Vec2D tile_clicked = Engine::get_map_viewer()->get_map()->pixel_to_tile(Vec2D(event.to.x, event.to.y));
            LOG(INFO) << "iteracting with tile " << tile_clicked.to_string();
            auto objects = Engine::get_objects_at(tile_clicked);
            if (objects.size() == 1) {
                callbackstate.register_number(objects[0]);
            } else if (objects.size() == 0) {
                LOG(INFO) << "Not objects to interact with";
            } else {
                LOG(WARNING) << "Not sure sprite object to switch to";
                callbackstate.register_number(objects[0]);
            }
        }
    ));

    EventManager &em = EventManager::get_instance();

    Typeface mytype("../fonts/hans-kendrick/HansKendrick-Regular.ttf");
    TextFont myfont(mytype, 18);
    Text mytext(&window, myfont, true);
    mytext.set_text("John");
    // referring to top left corner of text window
    mytext.move(TEXT_BORDER_WIDTH, TEXT_HEIGHT + TEXT_BORDER_WIDTH);
    auto window_size = window.get_size();
    mytext.resize(window_size.first-TEXT_BORDER_WIDTH, TEXT_HEIGHT + TEXT_BORDER_WIDTH);
    Engine::set_dialogue_box(&mytext);

    std::function<void(GameWindow*)> func = [&] (GameWindow* game_window) { 
        LOG(INFO) << "text window resizing"; 
        auto window_size = (*game_window).get_size();
        mytext.resize(window_size.first-TEXT_BORDER_WIDTH, TEXT_HEIGHT + TEXT_BORDER_WIDTH);
    };

    Lifeline text_lifeline = window.register_resize_handler(func);

    std::string editor;

    if (argc >= 2) {
        Engine::set_editor(argv[1]);
    };

    LongWalkChallenge long_walk_challenge(input_manager);
    callbackstate.spawn();
    long_walk_challenge.start();

    TextFont big_font(mytype, 50);
    Text cursor(&window, big_font, true);
    cursor.move(0, 0);
    cursor.resize(50, 50);
    cursor.set_text("<");
    Lifeline cursor_lifeline = input_manager->register_mouse_handler(filter({MOUSE_MOVE}, [&] (MouseInputEvent event) {cursor.move(event.to.x, event.to.y+25);}));
        
    while (!window.check_close()) {
        em.process_events();
        map_viewer.render_map();
        mytext.display();
        stoptext.display();
        runtext.display();
        cursor.display();
        window.swap_buffers();
        GameWindow::update();
    }

    return 0;
}
// 

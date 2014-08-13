#include <boost/filesystem.hpp>
#include <cassert>
#include <cmath>
#include <chrono>
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
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "api.hpp"
#include "button.hpp"
#include "component.hpp"
#include "engine.hpp"
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
#include "notification_bar.hpp"
#include "object_manager.hpp"
#include "sprite.hpp"
#include "typeface.hpp"
#include "text_font.hpp"
#include "text.hpp"
#include "walkability.hpp"

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

static std::mt19937 random_generator;

int create_sprite(Interpreter &interpreter) {
    LOG(INFO) << "Creating sprite";

    glm::ivec2 start(4, 15);

    // Registering new sprite with game engine
    auto new_sprite(make_shared<Sprite>(start, "John", Walkability::BLOCKED, 4));

    LOG(INFO) << "Adding sprite";
    ObjectManager::get_instance().add_object(new_sprite);
    Engine::get_map_viewer()->get_map()->add_sprite(new_sprite->get_id());

    Engine::get_map_viewer()->set_map_focus_object(new_sprite->get_id());
    LOG(INFO) << "Creating sprite wrapper";
    LOG(INFO) << "ID " << new_sprite->get_id();

    // Register user controled sprite
    // Yes, this is a memory leak. Deal with it.
    auto *a_thing(new Entity(start, new_sprite->get_name(), new_sprite->get_id()));

    LOG(INFO) << "Registering sprite";
    new_sprite->daemon = std::make_unique<LockableEntityThread>(interpreter.register_entity(*a_thing));
    LOG(INFO) << "Done!";

    return new_sprite->get_id();
}

class CallbackState {
    public:
        CallbackState(Interpreter &interpreter, std::string name):
            interpreter(interpreter),
            name(name) {
        }

        void register_number_key(unsigned int key) {
            LOG(INFO) << "changing focus to " << key;

            // Note: unsigned
            if (key >= key_to_id.size()) {
                LOG(INFO) << "changing focus aborted; no such id";
                return;
            }

            Engine::get_map_viewer()->set_map_focus_object(key_to_id[key]);
        }

        void register_number_id(int id) {
            LOG(INFO) << "changing focus to " << id;

            //TODO: handle incorrect ID

            Engine::get_map_viewer()->set_map_focus_object(id);
        }

        int spawn() {
            int id = create_sprite(interpreter);
            key_to_id.push_back(id);
            return id;
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

        void man_move(glm::vec2 direction) {
            auto id = Engine::get_map_viewer()->get_map_focus_object();
            Engine::move_sprite(id, direction);
        }

        void monologue () {
            auto id = Engine::get_map_viewer()->get_map_focus_object();
            auto location =  Engine::find_object(id);
            std::cout << "You are at " << location.x << ", " << location.y << std::endl;
        }

    private:
        Interpreter &interpreter;
        std::string name;
        std::vector<int> key_to_id;
};


int main(int argc, const char *argv[]) {
    // allows you to pass an alternative text editor to app, otherwise
    // defaults to gedit
    if (argc >= 2) {
        Engine::set_editor(argv[1]);
    };
    // TODO: Support no window
    // Can't do this cleanly at the moment as the MapViewer needs the window instance....

    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();

    int map_width(32);
    int map_height(32);
    GameWindow window(int(float(map_width)  * Engine::get_actual_tile_size()),
                      int(float(map_height) * Engine::get_actual_tile_size()),
                      false);

    window.use_context();

    Interpreter interpreter(boost::filesystem::absolute("python_embed/wrapper_functions.so").normalize());
  
   
    InputManager* input_manager = window.get_input_manager();
    GUIManager gui_manager;

    MapViewer map_viewer(&window, &gui_manager);
    Engine::set_map_viewer(&map_viewer);

    ChallengeBase base_challenge(&interpreter, &gui_manager);
    base_challenge.create_gui();
    map_viewer.set_map(base_challenge->get_map());

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
        [&] (KeyboardInputEvent) { callbackstate.man_move(glm::ivec2( 0, 1)); }
    ));

    Lifeline down_callback = input_manager->register_keyboard_handler(filter(
        {ANY_OF({KEY_HELD}), KEY({"Down", "S"})},
        [&] (KeyboardInputEvent) { callbackstate.man_move(glm::ivec2( 0, -1)); }
    ));

    Lifeline right_callback = input_manager->register_keyboard_handler(filter(
        {ANY_OF({KEY_HELD}), KEY({"Right", "D"})},
        [&] (KeyboardInputEvent) { callbackstate.man_move(glm::ivec2( 1,  0)); }
    ));

    Lifeline left_callback = input_manager->register_keyboard_handler(filter(
        {ANY_OF({KEY_HELD}), KEY({"Left", "A"})},
        [&] (KeyboardInputEvent) { callbackstate.man_move(glm::ivec2(-1,  0)); }
    ));

    Lifeline monologue_callback = input_manager->register_keyboard_handler(filter(
        {ANY_OF({KEY_PRESS}), KEY("M")},
        [&] (KeyboardInputEvent) { callbackstate.monologue(); }
    ));

    Lifeline mouse_button_lifeline = input_manager->register_mouse_handler(
        filter({ANY_OF({ MOUSE_RELEASE})}, [&] (MouseInputEvent event) {
            gui_manager.mouse_callback_function(event);
        })
    );


    std::vector<Lifeline> digit_callbacks;
    for (unsigned int i=0; i<10; ++i) {
        digit_callbacks.push_back(
            input_manager->register_keyboard_handler(filter(
                {KEY_PRESS, KEY(std::to_string(i))},
                [&, i] (KeyboardInputEvent) { callbackstate.register_number_key(i); }
            ))
        );
    }

    Lifeline switch_char = input_manager->register_mouse_handler(filter({ANY_OF({ MOUSE_RELEASE})},
        [&] (MouseInputEvent event) {
            LOG(INFO) << "mouse clicked on map at " << event.to.x << " " << event.to.y << " pixel";

            glm::vec2 tile_clicked(Engine::get_map_viewer()->pixel_to_tile(glm::ivec2(event.to.x, event.to.y)));
            LOG(INFO) << "interacting with tile " << tile_clicked.x << ", " << tile_clicked.y;

            auto sprites = Engine::get_sprites_at(tile_clicked);

            if (sprites.size() == 0) {
                LOG(INFO) << "No sprites to interact with";
            }
            else if (sprites.size() == 1) {
                callbackstate.register_number_id(sprites[0]);
            }
            else {
                LOG(WARNING) << "Not sure sprite object to switch to";
                callbackstate.register_number_id(sprites[0]);
            }
        }
    ));

    EventManager &em = EventManager::get_instance();

    std::function<void (GameWindow *)> func_char = [&] (GameWindow *) {
        LOG(INFO) << "text window resizing";
        Engine::text_updater();
    };

    Lifeline text_lifeline_char = window.register_resize_handler(func_char);

    int new_id = callbackstate.spawn();
    std::string bash_command =
        std::string("cp python_embed/scripts/long_walk_challenge.py python_embed/scripts/John_")
        + std::to_string(new_id) + std::string(".py");
    system(bash_command.c_str());



    LongWalkChallenge long_walk_challenge(input_manager);
    long_walk_challenge.start();


#ifdef USE_GLES
    TextFont big_font(Engine::get_game_typeface(), 50);
    Text cursor(&window, big_font, true);
    cursor.move(0, 0);
    cursor.resize(50, 50);
    cursor.set_text("<");

    Lifeline cursor_lifeline = input_manager->register_mouse_handler(
        filter({MOUSE_MOVE}, [&] (MouseInputEvent event) {
            cursor.move(event.to.x, event.to.y+25);
        })
    );
#endif

    auto last_clock(std::chrono::steady_clock::now());

    VLOG(3) << "{";
    while (!window.check_close()) {
        last_clock = std::chrono::steady_clock::now();

        do {
            VLOG(3) << "} SB | IM {";
            GameWindow::update();

            VLOG(3) << "} IM | EM {";
            em.process_events();
        } while (std::chrono::steady_clock::now() - last_clock < std::chrono::nanoseconds(1000000000 / 60));

        VLOG(3) << "} EM | RM {";
        map_viewer.render();

        VLOG(3) << "} RM | TD {";
        Engine::text_displayer();
        notification_bar.text_displayer();
#ifdef USE_GLES
        cursor.display();
#endif

        VLOG(3) << "} TD | SB {";
        window.swap_buffers();
    }
    VLOG(3) << "}";

    return 0;
}

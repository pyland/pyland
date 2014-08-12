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

    Map map("../resources/map0.tmx");

    Interpreter interpreter(boost::filesystem::absolute("python_embed/wrapper_functions.so").normalize());

    //BUILD the GUI
    GUIManager gui_manager;
    MapViewer map_viewer(&window, &gui_manager);
    map_viewer.set_map(&map);
    Engine::set_map_viewer(&map_viewer);

    //TODO : REMOVE THIS HACKY EDIT - done for the demo tomorrow
    TextFont buttonfont = Engine::get_game_font();
    Text stoptext(&window, buttonfont, true);
    Text runtext(&window, buttonfont, true);
    stoptext.set_text("Stop");
    runtext.set_text("Run");
    // referring to top left corner of text window
    stoptext.move(105, 240 + 20);
    runtext.move(5, 240 + 20);
    stoptext.resize(window.get_size().first-20, 80 + 20);
    runtext.resize(window.get_size().first-20, 80 + 20);

    // create first character, TODO: move this into challenge
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

    // build navigation bar buttons
    NotificationBar notification_bar;
    Engine::set_notification_bar(&notification_bar);

    sprite_window->add(run_button);
    sprite_window->add(stop_button);
    for (auto button: notification_bar.get_navigation_buttons()) {
        sprite_window->add(button);
    }


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


    // WARNING: Fragile reference capture
    Lifeline map_resize_lifeline = window.register_resize_handler([&] (GameWindow *) {
        map_viewer.resize();
    });

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
        stoptext.display();
        runtext.display();
        notification_bar.text_displayer();
        cursor.display();

        VLOG(3) << "} TD | SB {";
        window.swap_buffers();
    }
    VLOG(3) << "}";

    return 0;
}

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

#define TEXT_BORDER_WIDTH 20
#define TEXT_HEIGHT 80

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

    // TODO: move notification button to be better home

    Typeface notification_buttontype("../fonts/hans-kendrick/HansKendrick-Regular.ttf");
    TextFont notification_buttonfont(buttontype, 30);

    float button_size = 0.05f;
    std::pair<float,float> backward_loco(0.85f,0.05f);
    std::pair<float,float> forward_loco(0.95f,0.05f);

    std::shared_ptr<Button> backward_button = std::make_shared<Button>();
    backward_button->set_text("backward");
    backward_button->set_on_click([&] () {
        LOG(INFO) << "backward button pressed";
        Engine::move_notification(Direction::PREVIOUS);
    });
    backward_button->set_width(button_size);
    backward_button->set_height(button_size);
    backward_button->set_y_offset(backward_loco.second);
    backward_button->set_x_offset(backward_loco.first);
    Text backward_text(&window, notification_buttonfont, true);
    backward_text.set_text("<-");
    backward_text.move_ratio(backward_loco.first, backward_loco.second);
    backward_text.resize_ratio(button_size,button_size);


    std::shared_ptr<Button> forward_button = std::make_shared<Button>();
    forward_button->set_text("forward");
    forward_button->set_on_click([&] () {
        LOG(INFO) << "forward button pressed";
        Engine::move_notification(Direction::NEXT);
    });
    forward_button->set_width(button_size);
    forward_button->set_height(button_size);
    forward_button->set_y_offset(forward_loco.second);
    forward_button->set_x_offset(forward_loco.first);
    Text forward_text(&window, notification_buttonfont, true);
    forward_text.set_text("->");
    forward_text.move_ratio(forward_loco.first,forward_loco.second);
    forward_text.resize_ratio(button_size,button_size);


    sprite_window->add(run_button);
    sprite_window->add(stop_button);
    sprite_window->add(backward_button);
    sprite_window->add(forward_button);

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




    MapViewer map_viewer(&window, &gui_manager);
    map_viewer.set_map(&map);
    Engine::set_map_viewer(&map_viewer);

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
            (gui_manager.*mouse_callback_function) (event);})
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
            LOG(INFO) << "iteracting with tile " << tile_clicked.x << ", " << tile_clicked.y;

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

    Typeface mytype("../fonts/hans-kendrick/HansKendrick-Regular.ttf");
    TextFont myfont(mytype, 18);
    Text mytext(&window, myfont, true);
    mytext.set_text("John");
    // referring to top left corner of text window
    mytext.move(TEXT_BORDER_WIDTH, TEXT_HEIGHT + TEXT_BORDER_WIDTH);
    auto window_size = window.get_size();
    mytext.resize(window_size.first-TEXT_BORDER_WIDTH, TEXT_HEIGHT + TEXT_BORDER_WIDTH);
    Engine::set_dialogue_box(&mytext);

    std::function<void(GameWindow *)> func = [&] (GameWindow *game_window) {
        LOG(INFO) << "text window resizing";
        auto window_size = (*game_window).get_size();
        mytext.resize(window_size.first-TEXT_BORDER_WIDTH, TEXT_HEIGHT + TEXT_BORDER_WIDTH);
    };

    Lifeline text_lifeline = window.register_resize_handler(func);

    std::function<void (GameWindow *)> func_char = [&] (GameWindow *) {
        LOG(INFO) << "text window resizing";
        Engine::text_updater();
    };

    Lifeline text_lifeline_char = window.register_resize_handler(func_char);

    if (argc >= 2) {
        Engine::set_editor(argv[1]);
    };

    int new_id = callbackstate.spawn();
    std::string bash_command =
        std::string("cp python_embed/scripts/long_walk_challenge.py python_embed/scripts/John_")
        + std::to_string(new_id) + std::string(".py");
    system(bash_command.c_str());
    LongWalkChallenge long_walk_challenge(input_manager);
    long_walk_challenge.start();

    TextFont big_font(mytype, 50);
    Text cursor(&window, big_font, true);
    cursor.move(0, 0);
    cursor.resize(50, 50);
    cursor.set_text("<");

    Lifeline cursor_lifeline = input_manager->register_mouse_handler(
        filter({MOUSE_MOVE}, [&] (MouseInputEvent event) {
            cursor.move(event.to.x, event.to.y+25);
        })
    );

    auto last_clock = std::chrono::steady_clock::now();
    auto average_time = std::chrono::steady_clock::duration(0);

    VLOG(3) << "{";
    while (!window.check_close()) {
        auto new_clock = std::chrono::steady_clock::now();
        auto new_time = new_clock - last_clock;
        last_clock = new_clock;
        average_time = std::chrono::duration_cast<std::chrono::steady_clock::duration>(
            average_time * 0.99 + new_time * 0.01
        );

        VLOG_EVERY_N(1, 10) << std::chrono::seconds(1) / average_time;

        VLOG(3) << "} SB | IM {";
        GameWindow::update();

        VLOG(3) << "} IM | EM {";
        em.process_events();

        VLOG(3) << "} EM | RM {";
        map_viewer.render();

        VLOG(3) << "} RM | TD {";
        mytext.display();
        Engine::text_displayer();
        stoptext.display();
        runtext.display();
        forward_text.display();
        backward_text.display();
        cursor.display();

        VLOG(3) << "} TD | SB {";
        window.swap_buffers();
    }
    VLOG(3) << "}";

    return 0;
}

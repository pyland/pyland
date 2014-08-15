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
#include "callback_state.hpp"
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

/// CREATE GLOBAL OBJECTS

    //Create the game window to present to the users
    GameWindow window(int(float(map_width)  * Engine::get_actual_tile_size()),
                      int(float(map_height) * Engine::get_actual_tile_size()),
                      false);
    window.use_context();

    //Create the interpreter
    Interpreter interpreter(boost::filesystem::absolute("python_embed/wrapper_functions.so").normalize());
    //Create the input manager
    InputManager* input_manager = window.get_input_manager();

    //Create the GUI manager
    GUIManager gui_manager;
    
    //Create the map viewer
    MapViewer map_viewer(&window, &gui_manager);
    Engine::set_map_viewer(&map_viewer);

    //Create the callbackstate
    CallbackState callbackstate;

    //Create the event manager
    EventManager &em = EventManager::get_instance();

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

    gui_manager.set_root(sprite_window);

    // build navigation bar buttons
    NotificationBar notification_bar;
    Engine::set_notification_bar(&notification_bar);
    //    SpriteSwitcher sprite_switcher;

    sprite_window->add(run_button);
    sprite_window->add(stop_button);


    // quick fix so buttons in correct location in initial window before gui_resize_func callback
    auto original_window_size = window.get_size();
    sprite_window->set_width_pixels(original_window_size.first);
    sprite_window->set_height_pixels(original_window_size.second);

    gui_manager.parse_components();

    //The GUI resize function
    std::function<void(GameWindow*)> gui_resize_func = [&] (GameWindow* game_window) {
        LOG(INFO) << "GUI resizing";
        auto window_size = (*game_window).get_size();
        sprite_window->set_width_pixels(window_size.first);
        sprite_window->set_height_pixels(window_size.second);
        gui_manager.parse_components();
    };
    Lifeline gui_resize_lifeline = window.register_resize_handler(gui_resize_func);


    //The callbacks
    // WARNING: Fragile reference capture
    Lifeline map_resize_lifeline = window.register_resize_handler([&] (GameWindow *) {
        map_viewer.resize();
    });


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

    std::function<void (GameWindow *)> func_char = [&] (GameWindow *) {
        LOG(INFO) << "text window resizing";
        Engine::text_updater();
    };

    Lifeline text_lifeline_char = window.register_resize_handler(func_char);

    //TODO: CHallenge specific
    //default script

    
    int new_id = callbackstate.spawn(); //create a character
    std::string bash_command =
        std::string("cp python_embed/scripts/long_walk_challenge.py python_embed/scripts/John_")
        + std::to_string(new_id) + std::string(".py");
    system(bash_command.c_str());


    //Run the map
    bool run_game = true;
    while(!window.check_close() && run_game) {   

        //Setup challenge
        ChallengeData* challenge_data = new ChallengeData(
                                                          std::string("../resources/map0.tmx"),
                                                          &interpreter,
                                                          &gui_manager,
                                                          &window,
                                                          input_manager,
                                                          &notification_bar);

        LongWalkChallenge long_walk_challenge(challenge_data);
        long_walk_challenge.start();


        //Run the challenge - returns after challenge completes
        long_walk_challenge.run();

        //Clean up after the challenge
        em.flush();
    }

    return 0;
}

#define GLM_FORCE_RADIANS

#include <glog/logging.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <chrono>
#include <functional>
#include <glm/vec2.hpp>
#include <iostream>
#include <memory>
#include <random>
#include <ratio>
#include <string>
#include <utility>
#include <vector>

#include "callback_state.hpp"
#include "challenge_data.hpp"
#include "engine.hpp"
#include "event_manager.hpp"
#include "game_window.hpp"
#include "button.hpp"
#include "gui_manager.hpp"
#include "gui_window.hpp"
#include "filters.hpp"
#include "input_manager.hpp"
#include "keyboard_input_event.hpp"
#include "mouse_input_event.hpp"
#include "mouse_state.hpp"
#include "lifeline.hpp"
#include "long_walk_challenge.hpp"
#include "map_viewer.hpp"
#include "notification_bar.hpp"
#include "python_embed/interpreter.hpp"


using namespace std;

static std::mt19937 random_generator;

int main(int argc, const char *argv[]) {
    std::string map_path("../maps/map0.tmx");

    // allows you to pass an alternative text editor to app, otherwise
    // defaults to gedit. Also allows specification of a map file.
    switch (argc) {
        default:
            std::cout << "Usage: " << argv[0] << " [EDITOR] [MAP]" << std::endl;
            return 1;

        // The lack of break statements is not an error!!!
        case 3:
            map_path = std::string(argv[2]);
        case 2:
            Engine::set_editor(argv[1]);
        case 1:
            break;
    }

    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();

    /// CREATE GLOBAL OBJECTS

    //Create the game window to present to the users
    GameWindow window(800, 600, false);
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


    Lifeline zoom_in_callback = input_manager->register_keyboard_handler(filter(
        {ANY_OF({KEY_HELD}), KEY("=")},
        [&] (KeyboardInputEvent) { Engine::set_global_scale(Engine::get_global_scale() * 1.01f); }
    ));

    Lifeline zoom_out_callback = input_manager->register_keyboard_handler(filter(
        {ANY_OF({KEY_HELD}), KEY("-")},
        [&] (KeyboardInputEvent) { Engine::set_global_scale(Engine::get_global_scale() / 1.01f); }
    ));


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

    //Run the map
    bool run_game = true;

    while(!window.check_close() && run_game) {
        //Setup challenge
        ChallengeData *challenge_data(new ChallengeData(
            map_path,
            &interpreter,
            &gui_manager,
            &window,
            input_manager,
            &notification_bar
        ));

        LongWalkChallenge long_walk_challenge(challenge_data);
        long_walk_challenge.start();


        //Run the challenge - returns after challenge completes
        #ifdef USE_GLES
            TextFont big_font(Engine::get_game_typeface(), 50);
            Text cursor(challenge_data->game_window, big_font, true);
            cursor.move(0, 0);
            cursor.resize(50, 50);
            cursor.set_text("<");

            Lifeline cursor_lifeline(challenge_data->input_manager->register_mouse_handler(
                filter({MOUSE_MOVE}, [&] (MouseInputEvent event) {
                    cursor.move(event.to.x, event.to.y+25);
                })
            ));
        #endif

        auto last_clock(std::chrono::steady_clock::now());

        VLOG(3) << "{";
        while (!challenge_data->game_window->check_close()) {
            last_clock = std::chrono::steady_clock::now();

            VLOG(3) << "} SB | IM {";
            GameWindow::update();

            VLOG(3) << "} IM | EM {";

            do {
                EventManager::get_instance().process_events();
            } while (
                  std::chrono::steady_clock::now() - last_clock
                < std::chrono::nanoseconds(1000000000 / 60)
            );

            VLOG(3) << "} EM | RM {";
            Engine::get_map_viewer()->render();

            VLOG(3) << "} RM | TD {";
            Engine::text_displayer();
            challenge_data->notification_bar->text_displayer();

            #ifdef USE_GLES
                cursor.display();
            #endif

            VLOG(3) << "} TD | SB {";
            challenge_data->game_window->swap_buffers();
        }

        VLOG(3) << "}";

        //Clean up after the challenge - additional, non-challenge clean-up
        em.flush();

    }

    return 0;
}

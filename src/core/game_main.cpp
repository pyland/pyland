#define GLM_FORCE_RADIANS
#include <deque>
#include <fstream>
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
#include <sstream>
#include <utility>
#include <vector>

#include "game_main.hpp"

#include "callback_state.hpp"
#include "challenge_data.hpp"
#include "config.hpp"
#include "engine.hpp"
#include "event_manager.hpp"
#include "filters.hpp"
#include "input_handler.hpp"
#include "input_manager.hpp"
#include "interpreter.hpp"
#include "keyboard_input_event.hpp"
#include "lifeline.hpp"
#include "mouse_cursor.hpp"
#include "mouse_input_event.hpp"
#include "mouse_state.hpp"

#ifdef USE_GLES
#include "typeface.hpp"
#include "text_font.hpp"
#include "text.hpp"
#endif

//This class sets up the game and contains the game loop

using namespace std;

static std::mt19937 random_generator;

GameMain::GameMain(int &argc, char **argv):

    embedWindow(argc, argv, this),
    interpreter(boost::filesystem::absolute("python_embed/wrapper_functions.so").normalize()),
    callbackstate(),
    em(EventManager::get_instance()),
    changing_challenge(false),
    player_name("???")

{
    LOG(INFO) << "Constructing GameMain..." << endl;
    gui = new GUIMain(&embedWindow);

    Config::json j = Config::get_instance();
    /// CREATE GLOBAL OBJECTS

    //Create the input manager
    input_manager = embedWindow.get_input_manager();

    //The GUI resize function
    original_window_size = embedWindow.get_resolution();
    gui_resize_func = [&] (GameWindow* game_window)
    {
        LOG(INFO) << "GUI resizing";

        auto window_size = game_window->get_resolution();

        gui->get_gui_window()->set_width_pixels(window_size.first);
        gui->get_gui_window()->set_height_pixels(window_size.second);
        gui->refresh_gui();

        original_window_size = window_size;
    };

    //Register InputHandler callbacks TODO: Move this out to Python as neccesary!
    InputHandler::get_instance()->register_input_callback(InputHandler::INPUT_TOGGLE_SPEED, Engine::trigger_speed);
    InputHandler::get_instance()->register_input_callback(InputHandler::INPUT_CURRENT_SCRIPT, [] () {Engine::trigger_run(0); });
    InputHandler::get_instance()->register_input_callback(InputHandler::INPUT_ONE, [] () {Engine::trigger_run(1); });
    InputHandler::get_instance()->register_input_callback(InputHandler::INPUT_TWO, [] () {Engine::trigger_run(2); });
    InputHandler::get_instance()->register_input_callback(InputHandler::INPUT_THREE, [] () {Engine::trigger_run(3); });
    InputHandler::get_instance()->register_input_callback(InputHandler::INPUT_FOUR, [] () {Engine::trigger_run(4); });
    InputHandler::get_instance()->register_input_callback(InputHandler::INPUT_FIVE, [] () {Engine::trigger_run(5); });
    InputHandler::get_instance()->register_input_callback(InputHandler::INPUT_SIX, [] () {Engine::trigger_run(6); });
    InputHandler::get_instance()->register_input_callback(InputHandler::INPUT_SEVEN, [] () {Engine::trigger_run(7); });
    InputHandler::get_instance()->register_input_callback(InputHandler::INPUT_EIGHT, [] () {Engine::trigger_run(8); });
    InputHandler::get_instance()->register_input_callback(InputHandler::INPUT_NINE, [] () {Engine::trigger_run(9); });
    InputHandler::get_instance()->register_input_callback(InputHandler::INPUT_SWITCH, Engine::focus_next);

    gui_resize_lifeline = embedWindow.register_resize_handler(gui_resize_func);

    //The callbacks
    // WARNING: Fragile reference capture
    map_resize_lifeline = embedWindow.register_resize_handler([&] (GameWindow *)
    {
        gui->get_map_viewer()->resize();
    });

    up_callback = input_manager->register_keyboard_handler(filter(
    {KEY_HELD, REJECT(MODIFIER({"Left Shift", "Right Shift"})), KEY({"Up", "W"})},
    [&] (KeyboardInputEvent)
    {
        InputHandler::get_instance()->run_list(InputHandler::INPUT_UP); //Run this list of events registered against run in the input handler
    }
    ));

    down_callback = input_manager->register_keyboard_handler(filter(
    {KEY_HELD, REJECT(MODIFIER({"Left Shift", "Right Shift"})), KEY({"Down", "S"})},
    [&] (KeyboardInputEvent)
    {
        InputHandler::get_instance()->run_list(InputHandler::INPUT_DOWN); //Run this list of events registered against run in the input handler
    }
    ));

    right_callback = input_manager->register_keyboard_handler(filter(
    {KEY_HELD, REJECT(MODIFIER({"Left Shift", "Right Shift"})), KEY({"Right", "D"})},
    [&] (KeyboardInputEvent)
    {
        InputHandler::get_instance()->run_list(InputHandler::INPUT_RIGHT); //Run this list of events registered against run in the input handler
    }
    ));

    left_callback = input_manager->register_keyboard_handler(filter(
    {KEY_HELD, REJECT(MODIFIER({"Left Shift", "Right Shift"})), KEY({"Left", "A"})},
    [&] (KeyboardInputEvent)
    {
        InputHandler::get_instance()->run_list(InputHandler::INPUT_LEFT); //Run this list of events registered against run in the input handler
    }
    ));

    right_select_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, REJECT(MODIFIER({"Left Shift", "Right Shift"})), KEY({"Right", "D"})},
    [&] (KeyboardInputEvent)
    {
        if (Engine::is_bar_with_options_open()){
            gui->toggle_selection_notification_bar_with_options();
        }
    }
    ));

    left_select_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, REJECT(MODIFIER({"Left Shift", "Right Shift"})), KEY({"Left", "A"})},
    [&] (KeyboardInputEvent)
    {
        if (Engine::is_bar_with_options_open()){
            gui->toggle_selection_notification_bar_with_options();
        }
    }
    ));

    run_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, KEY("Space")},
    [&] (KeyboardInputEvent)
    {
        InputHandler::get_instance()->run_list(InputHandler::INPUT_CURRENT_SCRIPT);
    }
    ));

    speed_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, KEY({"Left Shift", "Right Shift"})},
    [&] (KeyboardInputEvent)
    {
        InputHandler::get_instance()->run_list(InputHandler::INPUT_TOGGLE_SPEED);
    }
    ));

    switch_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, KEY("Tab")},
    [&] (KeyboardInputEvent)
    {
        InputHandler::get_instance()->run_list(InputHandler::INPUT_SWITCH);
    }
    ));

    action_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, KEY("Return")},
    [&] (KeyboardInputEvent)
    {
        if (Engine::is_bar_with_options_open()){
            gui->proceed_notification_bar_with_options();
        }
        else if(Engine::is_bar_open()){
            gui->proceed_notification_bar();
        }
        else if(Engine::is_external_help_open()){
            gui->proceed_external_script_help();
        }
        else {
            InputHandler::get_instance()->run_list(InputHandler::INPUT_ACTION);
        }
    }
    ));

    script1_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, KEY("1")},
    [&] (KeyboardInputEvent)
    {
        InputHandler::get_instance()->run_list(InputHandler::INPUT_ONE);
    }
    ));

    script2_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, KEY("2")},
    [&] (KeyboardInputEvent)
    {
        InputHandler::get_instance()->run_list(InputHandler::INPUT_TWO);
    }
    ));

    script3_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, KEY("3")},
    [&] (KeyboardInputEvent)
    {
        InputHandler::get_instance()->run_list(InputHandler::INPUT_THREE);
    }
    ));

    script4_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, KEY("4")},
    [&] (KeyboardInputEvent)
    {
        InputHandler::get_instance()->run_list(InputHandler::INPUT_FOUR);
    }
    ));

    script5_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, KEY("5")},
    [&] (KeyboardInputEvent)
    {
        InputHandler::get_instance()->run_list(InputHandler::INPUT_FIVE);
    }
    ));

    script6_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, KEY("6")},
    [&] (KeyboardInputEvent)
    {
        InputHandler::get_instance()->run_list(InputHandler::INPUT_SIX);
    }
    ));

    script7_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, KEY("7")},
    [&] (KeyboardInputEvent)
    {
        InputHandler::get_instance()->run_list(InputHandler::INPUT_SEVEN);
    }
    ));

    script8_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, KEY("8")},
    [&] (KeyboardInputEvent)
    {
        InputHandler::get_instance()->run_list(InputHandler::INPUT_EIGHT);
    }
    ));

    script9_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, KEY("9")},
    [&] (KeyboardInputEvent)
    {
        InputHandler::get_instance()->run_list(InputHandler::INPUT_NINE);
    }
    ));

    mouse_button_lifeline = input_manager->register_mouse_handler(filter(
    {MOUSE_RELEASE},
    [&] (MouseInputEvent event)
    {
        gui->get_gui_manager()->mouse_callback_function(event);
    }));

    for (unsigned int i=0; i<10; ++i)
    {
        digit_callbacks.push_back(
            input_manager->register_keyboard_handler(filter(
        {KEY_PRESS, KEY(std::to_string(i))},
        [&, i] (KeyboardInputEvent)
        {
            callbackstate.register_number_key(i);
        }
        ))
        );
    }

    switch_char = input_manager->register_mouse_handler(filter(
    {MOUSE_RELEASE},
    [&] (MouseInputEvent event)
    {
        LOG(INFO) << "mouse clicked on map at " << event.to.x << " " << event.to.y << " pixel";

        glm::vec2 tile_clicked(Engine::get_map_viewer()->pixel_to_tile(glm::ivec2(event.to.x, event.to.y)));
        LOG(INFO) << "interacting with tile " << tile_clicked.x << ", " << tile_clicked.y;

        auto sprites = Engine::get_objects_at(tile_clicked);

        if (sprites.size() == 0)
        {
            LOG(INFO) << "No sprites to interact with";
        }
        else if (sprites.size() == 1)
        {
            //InputHandler::get_instance()->run_list(InputHandler::INPUT_CLICK);
            //callbackstate.register_number_id(sprites[0]);
        }
        else
        {
            LOG(WARNING) << "Not sure sprite object to switch to";
            //callbackstate.register_number_id(sprites[0]);
        }
    }
    ));

    func_char = [&] (GameWindow *)
    {
        LOG(INFO) << "text embedWindow resizing";
        //Engine::text_updater(); BLEH TODO: Work out what this does and if neccesary
    };

    text_lifeline_char = embedWindow.register_resize_handler(func_char);

    //Run the map
    run_game = true;
    cursor = new MouseCursor(&embedWindow);

    Engine::set_game_main(this);

    //Setup challenge
    challenge_data = (new ChallengeData(
                          "",
                          &interpreter,
                          gui->get_gui_manager(),
                          &embedWindow,
                          input_manager
                        ));
    challenge_data->run_challenge = true;

    std::string challenge_name = j["files"]["level_location"];
    std::string level_folder = j["files"]["level_folder"];
    challenge_data->map_name = level_folder + challenge_name + "/layout.tmx";
    challenge_data->level_location = challenge_name;
    challenge = new Challenge(challenge_data, gui);

    Engine::set_challenge(challenge);

    last_clock = (std::chrono::steady_clock::now());
    gui->refresh_gui();


    run_game = true;

    //Run the challenge - returns after challenge completes
    embedWindow.execute_app();

    LOG(INFO) << "Constructed GameMain" << endl;

}

GameMain::~GameMain()
{
    LOG(INFO) << "Destructing GameMain..." << endl;
    em->flush_and_disable(interpreter.interpreter_context);
    delete challenge;
    em->reenable();

    delete challenge_data;
    delete cursor;
    LOG(INFO) << "Destructed GameMain..." << endl;
}

std::string GameMain::get_current_challenge(){
    return challenge_data->level_location;
}

void GameMain::game_loop(bool showMouse)
{
    if(changing_challenge) {
        change_challenge(next_challenge);
        challenge_data->run_challenge = false;
        em->flush_and_disable(interpreter.interpreter_context);
        delete challenge;
        {
            lock::GIL raii_gil(interpreter.interpreter_context);
            delete gui;
            InputHandler::get_instance()->flush_all();
        }
        gui = new GUIMain(&embedWindow);
        auto window_size = embedWindow.get_resolution();
        gui->get_gui_window()->set_width_pixels(window_size.first);
        gui->get_gui_window()->set_height_pixels(window_size.second);
        gui->refresh_gui();

        //TODO: move this out to python, as this doesn't need to be done here and in the contstructor
        InputHandler::get_instance()->register_input_callback(InputHandler::INPUT_TOGGLE_SPEED, Engine::trigger_speed);
        InputHandler::get_instance()->register_input_callback(InputHandler::INPUT_CURRENT_SCRIPT, [] () {Engine::trigger_run(0); });
        InputHandler::get_instance()->register_input_callback(InputHandler::INPUT_ONE, [] () {Engine::trigger_run(1); });
        InputHandler::get_instance()->register_input_callback(InputHandler::INPUT_TWO, [] () {Engine::trigger_run(2); });
        InputHandler::get_instance()->register_input_callback(InputHandler::INPUT_THREE, [] () {Engine::trigger_run(3); });
        InputHandler::get_instance()->register_input_callback(InputHandler::INPUT_FOUR, [] () {Engine::trigger_run(4); });
        InputHandler::get_instance()->register_input_callback(InputHandler::INPUT_FIVE, [] () {Engine::trigger_run(5); });
        InputHandler::get_instance()->register_input_callback(InputHandler::INPUT_SIX, [] () {Engine::trigger_run(6); });
        InputHandler::get_instance()->register_input_callback(InputHandler::INPUT_SEVEN, [] () {Engine::trigger_run(7); });
        InputHandler::get_instance()->register_input_callback(InputHandler::INPUT_EIGHT, [] () {Engine::trigger_run(8); });
        InputHandler::get_instance()->register_input_callback(InputHandler::INPUT_NINE, [] () {Engine::trigger_run(9); });
        InputHandler::get_instance()->register_input_callback(InputHandler::INPUT_SWITCH, Engine::focus_next);

        em->reenable();

        Config::json j = Config::get_instance();
        std::string level_folder = j["files"]["level_folder"];
        challenge_data->map_name = level_folder + next_challenge + "/layout.tmx";
        challenge_data->level_location = next_challenge;
        challenge = new Challenge(challenge_data, gui);
        Engine::set_challenge(challenge);

        callbackstate.stop();
        challenge_data->run_challenge = true;

        changing_challenge = false;
    }
    else if ((!challenge_data->game_window->check_close()) && challenge_data->run_challenge && run_game)
    {
        //std::cout << "running game loop" << std::endl;

        last_clock = std::chrono::steady_clock::now();

        VLOG(3) << "} SB | IM {";
        GameWindow::update();

        VLOG(3) << "} IM | EM {";

        do
        {
            EventManager::get_instance()->process_events(interpreter.interpreter_context);
        }
        while (
            std::chrono::steady_clock::now() - last_clock
            < std::chrono::nanoseconds(1000000000 / 60)
        );

        VLOG(3) << "} EM | RM {";
        //std::cout << "calling render" << std::endl;
        Engine::get_map_viewer()->render();
        VLOG(3) << "} RM | TD {";

        // This is not an input event, because the map can move with
        // the mouse staying still.
        {
            std::pair<int,int> pixels = input_manager->get_mouse_pixels();
            glm::ivec2 tile(Engine::get_map_viewer()->pixel_to_tile( {pixels.first, pixels.second}));
            if (tile != tile_identifier_old_tile)
            {
                tile_identifier_old_tile = tile;
            }
        }

        //Only show SDL cursor on rapsberry pi, not required on desktop
        #ifdef USE_GLES
            //Display when mouse is over the SDL widget
            if (showMouse) {cursor->display();};
        #endif

        VLOG(3) << "} TD | SB {";
        challenge_data->game_window->swap_buffers();
    }
    else{
        std::cout << "not running game loop" << std::endl;
    }
    return;
}

void GameMain::change_challenge(std::string map_location) {
    next_challenge = map_location;
    changing_challenge = true; //Changing the challenge is handled in the main game loop so look there
}

GameWindow* GameMain::getGameWindow()
{
    return &embedWindow;
}

CallbackState GameMain::getCallbackState(){
    return callbackstate;
}

std::chrono::steady_clock::time_point GameMain::get_start_time(){
    return start_time;
}

//Switch the focus to the next playable object
void GameMain::focus_next(){
    gui->click_next_player();
}

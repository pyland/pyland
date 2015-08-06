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

    embedWindow(800, 600, argc, argv, this),
    interpreter(boost::filesystem::absolute("python_embed/wrapper_functions.so").normalize()),
    gui(&embedWindow),
    callbackstate(),
    em(EventManager::get_instance()),
    tile_identifier_text(&embedWindow, Engine::get_game_font(), false)

{
    LOG(INFO) << "Constructing GameMain..." << endl;

	nlohmann::json j = Config::get_instance();
    /// CREATE GLOBAL OBJECTS

    //Create the input manager
    input_manager = embedWindow.get_input_manager();

	//The GUI resize function
	original_window_size = embedWindow.get_size();
    gui_resize_func = [&] (GameWindow* game_window)
    {
        LOG(INFO) << "GUI resizing";

        auto window_size = (*game_window).get_size();

        gui.get_gui_window()->set_width_pixels(window_size.first);
        gui.get_gui_window()->set_height_pixels(window_size.second);
        gui.refresh_gui();

        original_window_size = window_size;
    };
    gui_resize_lifeline = embedWindow.register_resize_handler(gui_resize_func);

    //The callbacks
    // WARNING: Fragile reference capture
    map_resize_lifeline = embedWindow.register_resize_handler([&] (GameWindow *)
    {
        gui.get_map_viewer()->resize();
    });

    restart_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, KEY("R")},
    [&] (KeyboardInputEvent)
    {
        InputHandler::get_instance()->run_list(InputHandler::INPUT_RUN); //Run this list of events registered against run in the input handler
    }
    ));

    back_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, KEY("ESCAPE")},
    [&] (KeyboardInputEvent)
    {
        Engine::get_challenge()->event_finish.trigger();;
    }
    ));

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

    monologue_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, KEY("M")},
    [&] (KeyboardInputEvent)
    {
        callbackstate.monologue();
    }
    ));

    mouse_button_lifeline = input_manager->register_mouse_handler(filter(
    {MOUSE_RELEASE},
    [&] (MouseInputEvent event)
    {
        gui.get_gui_manager()->mouse_callback_function(event);
    }));

//    zoom_in_callback = input_manager->register_keyboard_handler(filter(
//    {KEY_HELD, KEY("=")},
//    [&] (KeyboardInputEvent)
//    {
//        Engine::set_global_scale(Engine::get_global_scale() * 1.01f);
//    }
//    ));
//
//    zoom_out_callback = input_manager->register_keyboard_handler(filter(
//    {KEY_HELD, KEY("-")},
//    [&] (KeyboardInputEvent)
//    {
//        Engine::set_global_scale(Engine::get_global_scale() / 1.01f);
//    }
//    ));

    zoom_zero_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, MODIFIER({"Left Ctrl", "Right Ctrl"}), KEY("0")},
    [&] (KeyboardInputEvent)
    {
        Engine::set_global_scale(1.0f);
    }
    ));

    help_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, MODIFIER({"Left Shift", "Right Shift"}), KEY("/")},
    [&] (KeyboardInputEvent)
    {
        auto id(Engine::get_map_viewer()->get_map_focus_object());
        auto active_player(ObjectManager::get_instance().get_object<MapObject>(id));

        Engine::print_dialogue(
            active_player->get_name(),
            "placeholder string"
                //active_player->get_instructions() TODO: BLEH remove this!
        );
    }
    ));

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
            callbackstate.register_number_id(sprites[0]);
        }
        else
        {
            LOG(WARNING) << "Not sure sprite object to switch to";
            callbackstate.register_number_id(sprites[0]);
        }
    }
    ));

    tile_identifier_text.move_ratio(1.0f, 0.0f);
    tile_identifier_text.resize(256, 64);
    tile_identifier_text.align_right();
    tile_identifier_text.vertical_align_bottom();
    tile_identifier_text.align_at_origin(true);
    tile_identifier_text.set_bloom_radius(5);
    tile_identifier_text.set_bloom_colour(0x00, 0x0, 0x00, 0xa0);
    tile_identifier_text.set_colour(0xff, 0xff, 0xff, 0xa8);
    tile_identifier_text.set_text("(?, ?)");

    func_char = [&] (GameWindow *)
    {
        LOG(INFO) << "text embedWindow resizing";
        //Engine::text_updater(); BLEH TODO: Work out what this does and if neccesary
    };

    text_lifeline_char = embedWindow.register_resize_handler(func_char);

    //Run the map
    run_game = true;
    cursor = new MouseCursor(&embedWindow);

    //Setup challenge
    challenge_data = (new ChallengeData(
                          "",
                          &interpreter,
                          gui.get_gui_manager(),
                          &embedWindow,
                          input_manager,
                          gui.get_notification_bar()
                          //0
                        ));
    challenge_data->run_challenge = true;
    challenge = pick_challenge(challenge_data);
    Engine::set_challenge(challenge);
    challenge->start();

    last_clock = (std::chrono::steady_clock::now());
    gui.refresh_gui();

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

void GameMain::game_loop(bool showMouse)
{
    if (!challenge_data->game_window->check_close() && challenge_data->run_challenge)
    {
        //callbackstate.man_move(glm::ivec2( 0, 1));
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
        Engine::get_map_viewer()->render();
        VLOG(3) << "} RM | TD {";
        //Engine::text_displayer(); TODO: work out what this did and if neccesary
        challenge_data->notification_bar->text_displayer();

        // This is not an input event, because the map can move with
        // the mouse staying still.
        {
            std::pair<int,int> pixels = input_manager->get_mouse_pixels();
            glm::ivec2 tile(Engine::get_map_viewer()->pixel_to_tile( {pixels.first, pixels.second}));
            if (tile != tile_identifier_old_tile)
            {
                tile_identifier_old_tile = tile;
                std::stringstream position;
                position << "(" << tile.x << ", " << tile.y << ")";

                tile_identifier_text.set_text(position.str());
            }
        }
        tile_identifier_text.display();

        //Only show SDL cursor on rapsberry pi, not required on desktop
        #ifdef USE_GLES
            //Display when mouse is over the SDL widget
            if (showMouse) {cursor->display();};
        #else
            ++showMouse; //TODO: find a nicer way to avoid unused variable warnings in desktop compiler :P
        #endif

        VLOG(3) << "} TD | SB {";
        challenge_data->game_window->swap_buffers();
    }
    else
    {
        em->flush_and_disable(interpreter.interpreter_context);
        delete challenge;
        em->reenable();

        challenge_data->run_challenge = true;
        challenge = pick_challenge(challenge_data);
        Engine::set_challenge(challenge);
        challenge->start();
        callbackstate.stop();
        //Update tool bar here
        //embedWindow.get_cur_game_init()->getMainWin()->updateToolBar();

    }
    return;
}

Challenge* GameMain::pick_challenge(ChallengeData* challenge_data) {

    //int next_challenge(challenge_data->next_challenge);
    Challenge *challenge(nullptr);
    nlohmann::json j = Config::get_instance();
    std::string map_name = j["files"]["full_level_location"];
    challenge_data->map_name = map_name + "/layout.tmx";
    challenge = new Challenge(challenge_data, &gui);

    return challenge;
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

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

#include "game_init.hpp"
#include "button.hpp"
#include "callback_state.hpp"
#include "challenge_data.hpp"
#include "config.hpp"
#include "engine.hpp"
#include "event_manager.hpp"
#include "filters.hpp"
#include "game_window.hpp"
#include "gui_manager.hpp"
#include "gui_window.hpp"
#include "input_handler.hpp"
#include "input_manager.hpp"
#include "interpreter.hpp"
#include "keyboard_input_event.hpp"
#include "lifeline.hpp"
#include "map_viewer.hpp"
#include "mouse_cursor.hpp"
#include "mouse_input_event.hpp"
#include "mouse_state.hpp"
#include "notification_bar.hpp"

#ifdef USE_GLES
#include "typeface.hpp"
#include "text_font.hpp"
#include "text.hpp"
#endif

#include "game_main.hpp"

using namespace std;

static std::mt19937 random_generator;

//Multiplication factors for converting game_window width and height to width and height for gui
const float x_scale = 1.0/680.0;
const float y_scale = 1.0/340.0;

//The maximum number of sprite head buttons to be displayed on the top
int button_max = 5;
const float button_spacing = 0.08f;

GameMain::GameMain(int &argc, char **argv):
    embedWindow(800, 600, argc, argv, this),
    interpreter(boost::filesystem::absolute("python_embed/wrapper_functions.so").normalize()),
    gui_manager(),
    callbackstate(),
    map_viewer(&embedWindow, &gui_manager),
    tile_identifier_text(&embedWindow, Engine::get_game_font(), false),
    paused(false),
    display_button_start(0)
{
    nlohmann::json j = Config::get_instance();
    LOG(INFO) << "Constructing GameMain..." << endl;

    /// CREATE GLOBAL OBJECTS

    //Create the game embedWindow to present to the users
    embedWindow.use_context();
    Engine::set_game_window(&embedWindow);

    //Create the input manager
    input_manager = embedWindow.get_input_manager();

    Engine::set_map_viewer(&map_viewer);

    //Create the event manager
    em = EventManager::get_instance();

    sprite_window = std::make_shared<GUIWindow>();
    sprite_window->set_visible(false);

    buttons.clear();
    gui_manager.set_root(sprite_window);

    notification_bar = new NotificationBar();

    Engine::set_notification_bar(notification_bar);

    pause_button = std::make_shared<Button>(ButtonType::Single);
    pause_button->set_picture("gui/game/pause");
    pause_button->set_text("");
    pause_button->set_width(0.15f);
    pause_button->set_height(0.35f);
    pause_button->set_y_offset(embedWindow.get_game_window_height()*y_scale);
    pause_button->set_x_offset(-0.05f);
    pause_button->set_on_click( [&] () {

        if(paused == false){
            paused = true;
            LOG(INFO) << "PAUSED";
            sprite_window->set_visible(true);
            refresh_gui();
            pause_menu();
        }
        else{
            paused = false;
            LOG(INFO) << "RESUMED";
            sprite_window->set_visible(false);
            refresh_gui();
        }
    });

    bag_button = std::make_shared<Button>(ButtonType::Single);
    bag_button->set_picture("gui/game/bag");
    bag_button->set_text("Bag");
    bag_button->set_width(0.15f);
    bag_button->set_height(0.35f);
    bag_button->set_y_offset(embedWindow.get_game_window_height()*y_scale);
    bag_button->set_x_offset(embedWindow.get_game_window_width()*x_scale);
    bag_button->set_on_click( [&] () {
        LOG(INFO) << "Bag opened";
    });

    sprite_window->add(pause_button);
    sprite_window->add(bag_button);

    original_window_size = embedWindow.get_size();
    sprite_window->set_width_pixels(original_window_size.first);
    sprite_window->set_height_pixels(original_window_size.second);

    //The GUI resize function
    gui_resize_func = [&] (GameWindow* game_window)
    {
        LOG(INFO) << "GUI resizing";
        auto window_size = (*game_window).get_size();
        sprite_window->set_width_pixels(window_size.first);
        sprite_window->set_height_pixels(window_size.second);
        gui_manager.parse_components();
    };
    gui_resize_lifeline = embedWindow.register_resize_handler(gui_resize_func);

    //The callbacks
    // WARNING: Fragile reference capture
    map_resize_lifeline = embedWindow.register_resize_handler([&] (GameWindow *)
    {
        map_viewer.resize();
    });

    restart_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, KEY("R")},
    [&] (KeyboardInputEvent)
    {
        InputHandler::get_instance()->run_list(InputHandler::INPUT_RUN); //Run this list of events registered against run in the input handler
    }
    ));

    Lifeline editor_callback = input_manager->register_keyboard_handler(filter(
        {KEY_PRESS, KEY("E")},
        [&] (KeyboardInputEvent) {
            Engine::open_editor();
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
        gui_manager.mouse_callback_function(event);
    }));

    zoom_in_callback = input_manager->register_keyboard_handler(filter(
    {KEY_HELD, KEY("=")},
    [&] (KeyboardInputEvent)
    {
        Engine::set_global_scale(Engine::get_global_scale() * 1.01f);
    }
    ));

    zoom_out_callback = input_manager->register_keyboard_handler(filter(
    {KEY_HELD, KEY("-")},
    [&] (KeyboardInputEvent)
    {
        Engine::set_global_scale(Engine::get_global_scale() / 1.01f);
    }
    ));

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
                          &gui_manager,
                          &embedWindow,
                          input_manager,
                          notification_bar
                          //0
                        ));
    challenge_data->run_challenge = true;
    challenge = pick_challenge(challenge_data);
    Engine::set_challenge(challenge);
    challenge->start();

    last_clock = (std::chrono::steady_clock::now());
    refresh_gui();

    //Run the challenge - returns after challenge completes
    embedWindow.execute_app();


    LOG(INFO) << "Constructed GameMain" << endl;

}

void GameMain::pause_menu()
{

}

void GameMain::add_button(std::string file_path, std::string name, std::function<void (void)> callback)
{
    if(buttons.size() % button_max == 0 && buttons.size() != 0)
    {
        cycle_button = std::make_shared<Button>(ButtonType::Single);
        cycle_button->set_picture("gui/highlight/selected_object");
        cycle_button->set_text("Cycle");
        cycle_button->set_width(0.15f);
        cycle_button->set_height(0.35f);
        cycle_button->set_y_offset(embedWindow.get_game_window_height()*y_scale);
        cycle_button->set_x_offset(embedWindow.get_game_window_width()*x_scale - button_max * button_spacing);
        cycle_button->set_on_click( [&] () {
            LOG(INFO) << "Switching bags";
        });
        sprite_window->add(cycle_button);

        for(int i=0; i<button_max; i++)
        {
            buttons[display_button_start+i]->set_visible(false);
        }
        refresh_gui();
    }
    std::shared_ptr<Button> new_button;
    new_button = std::make_shared<Button>(ButtonType::Single);
    buttons.push_back(new_button);
    new_button->set_picture(file_path);
    new_button->set_text(name);
    new_button->set_on_click(callback);
    new_button->set_width(0.15f);
    new_button->set_height(0.35f);

    //make space for previous buttons
    float org_x_location = embedWindow.get_game_window_width() * x_scale;

    new_button->set_x_offset(org_x_location - (((buttons.size()-1) % button_max) + 1) * button_spacing);
    new_button->set_y_offset(embedWindow.get_game_window_height() * y_scale);

    sprite_window->add(new_button);
    refresh_gui();
}

GameMain::~GameMain()
{
    LOG(INFO) << "Destructing GameMain..." << endl;
    em->flush_and_disable(interpreter.interpreter_context);
    delete challenge;
    em->reenable();

    buttons.clear();

    delete notification_bar;
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
    challenge = new Challenge(challenge_data, this);

    return challenge;
}

GameWindow* GameMain::getGameWindow()
{
    return &embedWindow;
}

void GameMain::refresh_gui()
{
    gui_manager.parse_components();
    //map_viewer.render();
}

CallbackState GameMain::getCallbackState(){
    return callbackstate;
}

std::chrono::steady_clock::time_point GameMain::get_start_time(){
    return start_time;
}

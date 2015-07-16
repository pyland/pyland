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
#include <sstream>
#include <utility>
#include <vector>

#include "game_init.hpp"
#include "button.hpp"
#include "callback_state.hpp"
#include "challenge_data.hpp"
#include "cutting_challenge.hpp"
#include "engine.hpp"
#include "event_manager.hpp"
#include "filters.hpp"
#include "final_challenge.hpp"
#include "game_window.hpp"
#include "gui_manager.hpp"
#include "gui_window.hpp"
#include "input_manager.hpp"
#include "interpreter.hpp"
#include "introduction_challenge.hpp"
#include "keyboard_input_event.hpp"
#include "lifeline.hpp"
#include "map_viewer.hpp"
#include "mouse_cursor.hpp"
#include "mouse_input_event.hpp"
#include "mouse_state.hpp"
#include "new_challenge.hpp"
#include "notification_bar.hpp"
#include "sprite.hpp"
#include "start_screen.hpp"

#ifdef USE_GLES
#include "typeface.hpp"
#include "text_font.hpp"
#include "text.hpp"
#endif

using namespace std;

static std::mt19937 random_generator;
Challenge* pick_challenge(ChallengeData* challenge_data);
int main(int argc, char *argv[]) {

    std::string map_path("../maps/start_screen.tmx");

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

    //Create the game embedWindow to present to the users
    GameWindow embedWindow(800, 600, false,argc, argv);
    embedWindow.use_context();
    Engine::set_game_window(&embedWindow);

    //Create the interpreter
    Interpreter interpreter(boost::filesystem::absolute("python_embed/wrapper_functions.so").normalize());
    //Create the input manager
    InputManager* input_manager = embedWindow.get_input_manager();

    //Create the GUI manager
    GUIManager gui_manager;

    //Create the map viewer
    MapViewer map_viewer(&embedWindow, &gui_manager);
    Engine::set_map_viewer(&map_viewer);

    //    void (GUIManager::*mouse_callback_function) (MouseInputEvent) = &GUIManager::mouse_callback_function;

    //TODO : REMOVE THIS HACKY EDIT - done for the demo tomorrow
    Typeface buttontype = Engine::get_game_typeface();
    TextFont buttonfont = Engine::get_game_font();

    std::shared_ptr<Text> stoptext = std::make_shared<Text>(&embedWindow, buttonfont, true);
    std::shared_ptr<Text> runtext = std::make_shared<Text>(&embedWindow, buttonfont, true);
    // referring to top left corner of text embedWindow
    //    stoptext.move(105, 240 + 20);
    //    runtext.move(5, 240 + 20);
    stoptext->set_text("Halt");
    runtext->set_text("Run");

    //Create the callbackstate
    CallbackState callbackstate;

    //Create the event manager
    EventManager &em = EventManager::get_instance();

    std::shared_ptr<GUIWindow> sprite_window = std::make_shared<GUIWindow>();;
    sprite_window->set_visible(false);
    std::shared_ptr<Button> run_button = std::make_shared<Button>();
    run_button->set_text(runtext);
    run_button->set_on_click([&] ()
    {
        LOG(ERROR) << "RUN";
        callbackstate.restart();
    });
    run_button->set_width(0.2f);
    run_button->set_height(0.2f);
    run_button->set_y_offset(0.8f);
    run_button->set_x_offset(0.0f);

    std::shared_ptr<Button> stop_button = std::make_shared<Button>();
    stop_button->set_text(stoptext);
    stop_button->set_on_click([&] ()
    {
        LOG(ERROR) << "STOP";
        callbackstate.stop();
    });
    stop_button->set_width(0.2f);
    stop_button->set_height(0.2f);
    stop_button->set_y_offset(0.67f);
    stop_button->set_x_offset(0.0f);


    gui_manager.set_root(sprite_window);

    // build navigation bar buttons
    NotificationBar notification_bar;
    Engine::set_notification_bar(&notification_bar);
    //    SpriteSwitcher sprite_switcher;

    sprite_window->add(run_button);
    sprite_window->add(stop_button);


    // quick fix so buttons in correct location in initial embedWindow before gui_resize_func callback
    auto original_window_size = embedWindow.get_size();
    sprite_window->set_width_pixels(original_window_size.first);
    sprite_window->set_height_pixels(original_window_size.second);

    gui_manager.parse_components();

    //The GUI resize function
    std::function<void(GameWindow*)> gui_resize_func = [&] (GameWindow* game_window)
    {
        LOG(INFO) << "GUI resizing";
        auto window_size = (*game_window).get_size();
        sprite_window->set_width_pixels(window_size.first);
        sprite_window->set_height_pixels(window_size.second);
        gui_manager.parse_components();
    };
    Lifeline gui_resize_lifeline = embedWindow.register_resize_handler(gui_resize_func);


    //The callbacks
    // WARNING: Fragile reference capture
    Lifeline map_resize_lifeline = embedWindow.register_resize_handler([&] (GameWindow *)
    {
        map_viewer.resize();
    });


    Lifeline stop_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, KEY("H")},
    [&] (KeyboardInputEvent)
    {
        callbackstate.stop();
    }
    ));

    Lifeline restart_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, KEY("R")},
    [&] (KeyboardInputEvent)
    {
        callbackstate.restart();
    }
    ));

    Lifeline editor_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, KEY("E")},
    [&] (KeyboardInputEvent)
    {
        auto id = Engine::get_map_viewer()->get_map_focus_object();
        auto active_player = ObjectManager::get_instance().get_object<Object>(id);
        if (!active_player)
        {
            return;
        }
        Engine::open_editor(active_player->get_name());
    }
    ));

    Lifeline back_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, KEY("ESCAPE")},
    [&] (KeyboardInputEvent)
    {
        Engine::get_challenge()->event_finish.trigger(0);;
    }
    ));


    std::chrono::steady_clock::time_point start_time;

    Lifeline fast_start_ease_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, KEY({"Left Shift", "Right Shift"})},
    [&] (KeyboardInputEvent)
    {
        start_time = std::chrono::steady_clock::now();
    }
    ));

    Lifeline fast_ease_callback = input_manager->register_keyboard_handler(filter(
    {KEY_HELD, KEY({"Left Shift", "Right Shift"})},
    [&] (KeyboardInputEvent)
    {
        auto now(std::chrono::steady_clock::now());
        auto time_passed = now - start_time;

        float completion(time_passed / std::chrono::duration<float>(6.0f));
        completion = std::min(completion, 1.0f);

        // Using an easing function from the internetz:
        //
        //     start + (c⁵ - 5·c⁴ + 5·c³) change
        //
        float eased(1.0f + 511.0f * (
                        + 1.0f * completion * completion * completion * completion * completion
                        - 5.0f * completion * completion * completion * completion
                        + 5.0f * completion * completion * completion
                    ));

        EventManager::get_instance().time.set_game_seconds_per_real_second(eased);
    }
    ));

    Lifeline fast_finish_ease_callback = input_manager->register_keyboard_handler(filter(
    {KEY_RELEASE, KEY({"Left Shift", "Right Shift"})},
    [&] (KeyboardInputEvent)
    {
        EventManager::get_instance().time.set_game_seconds_per_real_second(1.0);
    }
    ));


    Lifeline up_callback = input_manager->register_keyboard_handler(filter(
    {KEY_HELD, REJECT(MODIFIER({"Left Shift", "Right Shift"})), KEY({"Up", "W"})},
    [&] (KeyboardInputEvent)
    {
        callbackstate.man_move(glm::ivec2( 0, 1));
    }
    ));

    Lifeline down_callback = input_manager->register_keyboard_handler(filter(
    {KEY_HELD, REJECT(MODIFIER({"Left Shift", "Right Shift"})), KEY({"Down", "S"})},
    [&] (KeyboardInputEvent)
    {
        callbackstate.man_move(glm::ivec2( 0, -1));
    }
    ));

    Lifeline right_callback = input_manager->register_keyboard_handler(filter(
    {KEY_HELD, REJECT(MODIFIER({"Left Shift", "Right Shift"})), KEY({"Right", "D"})},
    [&] (KeyboardInputEvent)
    {
        callbackstate.man_move(glm::ivec2( 1,  0));
    }
    ));

    Lifeline left_callback = input_manager->register_keyboard_handler(filter(
    {KEY_HELD, REJECT(MODIFIER({"Left Shift", "Right Shift"})), KEY({"Left", "A"})},
    [&] (KeyboardInputEvent)
    {
        callbackstate.man_move(glm::ivec2(-1,  0));
    }
    ));

    Lifeline monologue_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, KEY("M")},
    [&] (KeyboardInputEvent)
    {
        callbackstate.monologue();
    }
    ));

    Lifeline mouse_button_lifeline = input_manager->register_mouse_handler(filter(
    {MOUSE_RELEASE},
    [&] (MouseInputEvent event)
    {
        gui_manager.mouse_callback_function(event);
    })
                                                                          );


    Lifeline zoom_in_callback = input_manager->register_keyboard_handler(filter(
    {KEY_HELD, KEY("=")},
    [&] (KeyboardInputEvent)
    {
        Engine::set_global_scale(Engine::get_global_scale() * 1.01f);
    }
    ));

    Lifeline zoom_out_callback = input_manager->register_keyboard_handler(filter(
    {KEY_HELD, KEY("-")},
    [&] (KeyboardInputEvent)
    {
        Engine::set_global_scale(Engine::get_global_scale() / 1.01f);
    }
    ));

    Lifeline zoom_zero_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, MODIFIER({"Left Ctrl", "Right Ctrl"}), KEY("0")},
    [&] (KeyboardInputEvent)
    {
        Engine::set_global_scale(1.0f);
    }
    ));


    Lifeline help_callback = input_manager->register_keyboard_handler(filter(
    {KEY_PRESS, MODIFIER({"Left Shift", "Right Shift"}), KEY("/")},
    [&] (KeyboardInputEvent)
    {
        auto id(Engine::get_map_viewer()->get_map_focus_object());
        auto active_player(ObjectManager::get_instance().get_object<Sprite>(id));

        Engine::print_dialogue(
            active_player->get_name(),
            active_player->get_instructions()
        );
    }
    ));


    std::vector<Lifeline> digit_callbacks;
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

    Lifeline switch_char = input_manager->register_mouse_handler(filter(
    {MOUSE_RELEASE},
    [&] (MouseInputEvent event)
    {
        LOG(INFO) << "mouse clicked on map at " << event.to.x << " " << event.to.y << " pixel";

        glm::vec2 tile_clicked(Engine::get_map_viewer()->pixel_to_tile(glm::ivec2(event.to.x, event.to.y)));
        LOG(INFO) << "interacting with tile " << tile_clicked.x << ", " << tile_clicked.y;

        auto sprites = Engine::get_sprites_at(tile_clicked);

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

    Text tile_identifier_text(&embedWindow, Engine::get_game_font(), false);
    tile_identifier_text.move_ratio(1.0f, 0.0f);
    tile_identifier_text.resize(256, 64);
    tile_identifier_text.align_right();
    tile_identifier_text.vertical_align_bottom();
    tile_identifier_text.align_at_origin(true);
    tile_identifier_text.set_bloom_radius(5);
    tile_identifier_text.set_bloom_colour(0x00, 0x0, 0x00, 0xa0);
    tile_identifier_text.set_colour(0xff, 0xff, 0xff, 0xa8);
    tile_identifier_text.set_text("(?, ?)");
    glm::ivec2 tile_identifier_old_tile;

    std::function<void (GameWindow *)> func_char = [&] (GameWindow *)
    {
        LOG(INFO) << "text embedWindow resizing";
        Engine::text_updater();
    };

    Lifeline text_lifeline_char = embedWindow.register_resize_handler(func_char);

    //Run the map
    bool run_game = true;

    //Setup challenge
    ChallengeData *challenge_data(new ChallengeData(
                                      map_path,
                                      &interpreter,
                                      &gui_manager,
                                      &embedWindow,
                                      input_manager,
                                      &notification_bar,
                                      0
                                  ));

    MouseCursor cursor(&embedWindow);
    //Run the challenge - returns after challenge completes

    while(!embedWindow.check_close() && run_game)
    {
        challenge_data->run_challenge = true;
        Challenge* challenge = pick_challenge(challenge_data);
        Engine::set_challenge(challenge);
        challenge->start();

        auto last_clock(std::chrono::steady_clock::now());

        //Run the challenge - returns after challenge completes
        VLOG(3) << "{";
        while (!challenge_data->game_window->check_close() && challenge_data->run_challenge)
        {
            last_clock = std::chrono::steady_clock::now();

            VLOG(3) << "} SB | IM {";
            GameWindow::update();

            VLOG(3) << "} IM | EM {";

            do
            {
                EventManager::get_instance().process_events();
            }
            while (
                std::chrono::steady_clock::now() - last_clock
                < std::chrono::nanoseconds(1000000000 / 60)
            );

            VLOG(3) << "} EM | RM {";
            Engine::get_map_viewer()->render();
            VLOG(3) << "} RM | TD {";
            Engine::text_displayer();
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

            cursor.display();

            VLOG(3) << "} TD | SB {";
            challenge_data->game_window->swap_buffers();
        }

        VLOG(3) << "}";

        // Clean up after the challenge - additional, non-challenge clean-up
        em.flush_and_disable();
        delete challenge;
        em.reenable();

    }

    //game_init(argc, argv);

    return 0;
}


Challenge* pick_challenge(ChallengeData* challenge_data)
{
    int next_challenge(challenge_data->next_challenge);
    Challenge *challenge(nullptr);
    std::string map_name = "";
    switch(next_challenge)
    {
    case 0:
        map_name ="../maps/start_screen.tmx";
        challenge_data->map_name = map_name;
        challenge = new StartScreen(challenge_data);
        break;
    case 1:
        map_name = "../maps/introduction.tmx";
        challenge_data->map_name = map_name;
        challenge = new IntroductionChallenge(challenge_data);
        break;
    case 2:
        map_name = "../maps/cutting_challenge.tmx";
        challenge_data->map_name = map_name;
        challenge = new CuttingChallenge(challenge_data);
        break;
    case 3:
        map_name = "../maps/final_challenge.tmx";
        challenge_data->map_name = map_name;
        challenge = new FinalChallenge(challenge_data);
        break;
    case 4:
        map_name = "../maps/new_challenge.tmx";
        challenge_data->map_name = map_name;
        challenge = new NewChallenge(challenge_data);
        break;
    default:
        break;
    }
    return challenge;
}

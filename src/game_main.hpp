#ifndef GAME_MAIN_H
#define GAME_MAIN_H

#include <string>
#include <memory>
#include "game_window.hpp"
#include "interpreter.hpp"
#include "gui_manager.hpp"
#include "callback_state.hpp"
#include "map_viewer.hpp"
#include "typeface.hpp"
#include "text_font.hpp"
#include "text.hpp"
#include "gui_window.hpp"
#include "button.hpp"
#include "notification_bar.hpp"

class Challenge;
class ChallengeData;
class InputManager;
class EventManager;

class GameMain{
private:
    std::string map_path;
    GameWindow embedWindow;
    Interpreter interpreter;
    InputManager* input_manager;
    GUIManager gui_manager;
    CallbackState callbackstate;
    MapViewer map_viewer;
    Typeface buttontype;
    TextFont buttonfont;

    std::shared_ptr<Text> stoptext;
    std::shared_ptr<Text> runtext;

    EventManager *em;

    std::shared_ptr<GUIWindow> sprite_window;
    std::shared_ptr<Button> run_button;
    std::shared_ptr<Button> stop_button;

    //NotificationBar notification_bar;


public:
    GameMain(int argc, char *argv[]);
    void game_loop();
    Challenge* pick_challenge(ChallengeData* challenge_data);

};

#endif // GAME_MAIN_H

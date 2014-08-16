#ifndef CHALLENGE_DATA_H
#define CHALLENGE_DATA_H

#include <memory>

#include "game_window.hpp"
#include "gui_manager.hpp"
#include "input_manager.hpp"
#include "interpreter.hpp"
#include "lifeline.hpp"
#include "map.hpp"
#include "map_viewer.hpp"
#include "notification_bar.hpp"
#include "sprite.hpp"

///
/// used to pass data to a challenge. Essentially collects together
/// all the pointers that we need to pass to a challenge to initialise it.
///
class ChallengeData {
    public:
        std::string map_name;
        Interpreter* interpreter;
        GUIManager* gui_manager;
        GameWindow* game_window;

        InputManager *input_manager;
        NotificationBar* notification_bar;

        ChallengeData(std::string map_name, Interpreter* interpreter, GUIManager* gui_manager, GameWindow* game_window, InputManager* input_manager, NotificationBar* notification_bar):
            map_name(map_name),
            interpreter(interpreter),
            gui_manager(gui_manager),
            game_window(game_window),
            input_manager(input_manager),
            notification_bar(notification_bar)
            {}
};

#endif

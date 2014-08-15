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
#include "player.hpp"
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

        std::shared_ptr<Player> player;
        InputManager *input_manager;
        NotificationBar* notification_bar;

        ChallengeData(std::string _map_name, Interpreter* _interpreter, GUIManager* _gui_manager, GameWindow* _game_window, std::shared_ptr<Player> _player, InputManager* _input_manager, NotificationBar* _notification_bar):
            interpreter(_interpreter),
            gui_manager(_gui_manager),
            game_window(_game_window),
            player(_player),
            input_manager(_input_manager),
            notification_bar(_notification_bar)
        {
            map_name = _map_name;
        }
};

#endif

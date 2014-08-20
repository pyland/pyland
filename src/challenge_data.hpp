#ifndef CHALLENGE_DATA_H
#define CHALLENGE_DATA_H

#include <string>

class Interpreter;
class GUIManager;
class GameWindow;
class InputManager;
class NotificationBar;

///
/// Wsed to pass data to a challenge. Essentially collects together
/// all the pointers that we need to pass to a challenge to initialise it.
/// Also used to determine if a challenge should be kept running and to determine
/// which challenge to load next, after this one completes.
///
///
class ChallengeData {
public:
    std::string map_name;
    Interpreter* interpreter;
    GUIManager* gui_manager;
    GameWindow* game_window;
    InputManager *input_manager;
    NotificationBar* notification_bar;
    int next_challenge;
    bool run_challenge;
    ChallengeData(std::string map_name,
                  Interpreter *interpreter,
                  GUIManager *gui_manager,
                  GameWindow *game_window,
                  InputManager *input_manager,
                  NotificationBar *notification_bar,
                  int next_challenge):

        map_name(map_name),
        interpreter(interpreter),
        gui_manager(gui_manager),
        game_window(game_window),
        input_manager(input_manager),
        notification_bar(notification_bar),
        next_challenge(next_challenge),
        run_challenge(true)
    {}
};

#endif

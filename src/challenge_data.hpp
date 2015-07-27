#ifndef CHALLENGE_DATA_H
#define CHALLENGE_DATA_H

#include <string>

class Interpreter;
class GUIManager;
class GameWindow;
class InputManager;
class NotificationBar;

///
/// Used to pass data to a challenge. Essentially collects together
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

    //next_challenge - should be a string
    bool run_challenge;

    //the constructor
    ChallengeData(std::string map_name,
                  Interpreter *interpreter,
                  GUIManager *gui_manager,
                  GameWindow *game_window,
                  InputManager *input_manager,
                  NotificationBar *notification_bar
                  //int next_challenge
                  ):

    //initialise the fields with the parameters passed to the constructor
        map_name(map_name),
        interpreter(interpreter),
        gui_manager(gui_manager),
        game_window(game_window),
        input_manager(input_manager),
        notification_bar(notification_bar),
        //next_challenge(next_challenge),
        run_challenge(true)
    {}
};

#endif

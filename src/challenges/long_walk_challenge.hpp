#ifndef LONG_WALK_CHALLENGE_H
#define LONG_WALK_CHALLENGE_H

#include <string>
#include <vector>
#include "challenge.hpp"
#include "dispatcher.hpp"
#include "game_window.hpp"
#include "gui_manager.hpp"
#include "input_manager.hpp"
#include "interpreter.hpp"
#include "lifeline.hpp"
#include "map.hpp"
#include "map_viewer.hpp"
#include "sprite.hpp"
#include "typeface.hpp"
#include "text_font.hpp"
#include "text.hpp"

class InputManager;

class LongWalkChallenge : public Challenge {
public:
    LongWalkChallenge(std::string map_name, Interpreter* _interpreter, GUIManager* _gui_manager, GameWindow* _game_window, InputManager* _input_manager, MapViewer* _map_viewer);

        virtual void start();
        virtual void finish();

    private:
        std::vector<std::shared_ptr<MapObject>> wall_path_medium_objects;
        std::vector<std::shared_ptr<MapObject>> wall_path_long_objects;

        std::vector<PositionDispatcher<int>::CallbackID> room_exit_first_callback;
        std::vector<PositionDispatcher<int>::CallbackID> wall_path_medium_callbacks;
        std::vector<PositionDispatcher<int>::CallbackID> wall_path_long_callbacks;

        Lifeline editor_lifeline;
};

#endif

#ifndef CHALLENGE_H
#define CHALLENGE_H

#include <string>

#include "game_window.hpp"
#include "gui_manager.hpp"
#include "input_manager.hpp"
#include "interpreter.hpp"
#include "map.hpp"
#include "sprite.hpp"
#include "typeface.hpp"
#include "text_font.hpp"
#include "text.hpp"

class InputManager;

class Challenge {
protected:

    Interpreter* interpreter;
    GUIManager* gui_manager;
    GameWindow* game_window;
    Map* map;
    Sprite* player;
    InputManager *input_manager;    

public:
    // TODO: Make the editor part of the core
    // engine, not the challenge class
    Challenge(std::string map_name, Interpreter* _interpreter, GUIManager* _gui_manager, GameWindow* _game_window, InputManager* input_manager);

    virtual ~Challenge();

    virtual void start() = 0;
    virtual void finish() = 0;


};

#endif

#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <boost/python/object_core.hpp>
#include <boost/python/list.hpp>
#include <string>

#include "input_handler.hpp"

class GUIMain;
///
/// This class is used as the python interface for general game-engine functionality.
/// The base game.py has the code which wraps around it and provides some additiondal functionality as well.
///
class GameEngine {

    private:
        GUIMain * gui_main;

    public:

        static int INPUT_UP() { return InputHandler::INPUT_UP; }
        static int INPUT_RIGHT() { return InputHandler::INPUT_RIGHT; }
        static int INPUT_DOWN() { return InputHandler::INPUT_DOWN; }
        static int INPUT_LEFT() { return InputHandler::INPUT_LEFT; }

        static int INPUT_RUN() { return InputHandler::INPUT_RUN; }
        static int INPUT_HALT() { return InputHandler::INPUT_HALT; }
        static int THE_AWNSER_TO_LIFE_THE_UNIVERSE_AND_EVERYTHING() { return 42; }

        GameEngine(GUIMain * _gui_main){
            gui_main = _gui_main;
        }

        ///
        /// Add an object to the game map at the given position.
        /// The name is used to give the object a name and the class_location string is
        /// used to find the correct Python class to wrap the object.
        ///
        boost::python::object add_object(std::string name, std::string class_location, int x, int y);

        ///
        /// Returns a list of python objects at the location given.
        /// Maybe should be implemented in python instead. (Which means that the python code should be aware of where
        /// everything is as well, may be too costly.)
        ///
        boost::python::list get_objects_at(int x, int y);

        ///
        /// Change to the level given. Unloads the current level and objects,
        /// loads in the new level and objects.
        ///
        ///
        void change_level(std::string level_location);

        ///
        /// Get the location of the level data in the file system relative to the game/levels folder.
        ///
        std::string get_level_location();

        ///
        /// Print a debug message for the python code. At the moment the debug_message is just squirted out into
        /// the log. It needs to be reviewed if this is the best way of doing things.
        ///
        void print_debug(std::string debug_message);

        ///
        /// To add a button to the challenge
        ///
        void add_button(std::string file_path, std::string name, PyObject* callback);

        void register_input_callback(int input_key, PyObject *input_callback);

        void play_music(std::string song_name);

        void print_terminal(std::string text, bool error);

        void set_ui_colours(int r1, int b1, int g1, int r2, int b2, int g2);

};

#endif

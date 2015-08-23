#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <boost/python/object_core.hpp>
#include <boost/python/list.hpp>
#include <string>

#include "input_handler.hpp"

class Challenge;
class GUIMain;
///
/// This class is used as the python interface for general game-engine functionality.
/// The base game.py has the code which wraps around it and provides some additiondal functionality as well.
///
class GameEngine {

    private:
        GUIMain * gui_main;
        unsigned int button_id;
        Challenge *challenge;

    public:

        static int INPUT_UP() { return InputHandler::INPUT_UP; }
        static int INPUT_RIGHT() { return InputHandler::INPUT_RIGHT; }
        static int INPUT_DOWN() { return InputHandler::INPUT_DOWN; }
        static int INPUT_LEFT() { return InputHandler::INPUT_LEFT; }

        static int INPUT_RUN() { return InputHandler::INPUT_RUN; }
        static int INPUT_HALT() { return InputHandler::INPUT_HALT; }
        static int INPUT_ACTION() { return InputHandler::INPUT_ACTION; }

        GameEngine(GUIMain * _gui_main){
            gui_main = _gui_main;
            button_id = 0;
        }

        static int TILE_TYPE_STANDARD() { return 0; }
        static int TILE_TYPE_SOLID() { return 1; }
        static int TILE_TYPE_WATER() { return 2; }

        GameEngine(GUIMain *_gui_main, Challenge *_challenge);
        ///
        /// Add an object to the game map at the given position.
        /// The name is used to give the object a name and the class_location string is
        /// used to find the correct Python class to wrap the object.
        ///
        boost::python::object create_object(std::string object_file_location, std::string object_name, int x, int y);

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
        void change_map(std::string level_location);

        int get_tile_type(int x, int y);

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
        /// Returns the button_id associated with the python player
        unsigned int add_button(std::string file_path, std::string name, PyObject* callback);

        ///
        /// Updates the player focus buttons with the newly
        /// focused player (associated with it's button id)
        void set_cur_player(unsigned int passing_button_id);

        ///
        /// Updates the player focus buttons with the newly
        /// changed player name (associated with it's button id)
        void update_player_name(std::string name, unsigned int passing_button_id);

        ///
        /// To add a dialogue to the notificaton bar
        /// options is a python dictionary of strings to callbacks
        ///
        void show_dialogue(std::string text, bool disable_scripting, PyObject *callback);
        void show_dialogue_with_options(std::string text, bool disable_scripting, PyObject *options);
        void show_external_script_help(std::string text, PyObject *callback);

        ///
        /// Register a callback against a given input
        ///
        void register_input_callback(int input_key, PyObject *input_callback);
        void flush_input_callback_list(int input_key);

        ///
        /// Play a given song from the music folder
        ///
        void play_music(std::string song_name);

        ///
        /// Returns a raw string of the json config
        ///
        std::string get_config();

        ///
        /// Change the settings of the PyScripter
        ///
        void show_py_scripter(PyObject* callback);
        void hide_py_scripter(PyObject* callback);
        void enable_py_scripter(PyObject* callback);
        void disable_py_scripter(PyObject* callback);
        void enable_script_editing(PyObject* callback);
        void disable_script_editing(PyObject* callback);
        void set_py_tabs(int val, PyObject* callback);
        void show_external_script(PyObject* confirm_callback, PyObject* cancel_callback, std::string external_dialogue, PyObject* script_init);
        //void hide_external_tab(PyObject* callback);

        ///
        /// Update the info bar with the current
        /// world/level/coins/totems
        ///
        void update_world(std::string text);
        void update_level(std::string text);
        void update_coins(int value);
        void update_totems(int value, bool show);

        /// Insert text to the current QsciScintilla
        /// text edtior
        ///
        void insert_to_scripter(std::string text);

        /// Clear all the text in the current QsciScintilla
        /// text edtior
        ///
        void clear_scripter(PyObject* callback);

        /// Get the text in the current QsciScintilla
        /// text edtior
        ///
        std::string get_script();

        /// Get the text in the external QsciScintilla
        /// text edtior tab
        ///
        std::string get_external_script();

        /// Print text to the QT terminal widget
        /// If error is True the text is red
        /// If error is False the text is black
        void print_terminal(std::string text, bool error);

        ///
        /// Update the QT interface with new colours
        /// (preferrably two shades of the same colour)
        void set_ui_colours(int r1, int b1, int g1, int r2, int b2, int g2);

        ///
        /// Updates the QT running button to reflect
        /// whether the current player is running a script
        void set_running();
        void set_finished();

        ///
        /// Run the currently active script
        /// (called when enter is pressed)
        void trigger_run();

        ///
        /// Get the index of the script to run for the player
        /// (this may be the currently open script
        /// or another script if a key binding was used)
        int get_run_script();

        bool is_solid(int x, int y);

        ///
        /// Force the config file to be loaded in again, useful for if you have changed any settings in it.
        ///
        void refresh_config();

        /// Adds a callback to the event_manager queue
        void add_event(PyObject *py_callback);

        ///get the name of the player currently playing, returns empty string if not known yet. Used by Python to keep track of the correct game save
        std::string get_player_name();

        void set_player_name(std::string player_name);
};

#endif

#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <boost/python/object_core.hpp>
#include <boost/python/list.hpp>
#include <string>

///
/// This class is used as the python interface for general game-engine functionality.
/// The base game.py has the code which wraps around it and provides some additiondal functionality as well.
///
class GameEngine {
    public:
        GameEngine();

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
        void add_button(std::string file_path, int button_type, PyObject* callback);
};

#endif

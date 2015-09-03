import os
import sys
import importlib
import sqlite3
import json
import threading
import collections

class Engine:
    """ This class is a python wrapper for all the engine features that are exposed to the game.

    As some features are implemented in python and some in C++, this consolidates both.
    """
    #Represents the cplusplus engine
    __cpp_engine = None
    #A dictonary of all the game objects in the level (maps from object_id to object)
    __game_objects = dict()
    #Represents the connections to the sqlite database
    conn = dict()

    #Whether the most recent script run gave an error
    __error = False

    __json_data = None

    def get_dialogue(self, level_name, identifier, escapes = dict()):
        """ Get the piece of dialoge requested form the database.

        Parameters
        ----------
        level_name : str
            The level the dialogue is associated with. For example, a dialogue in World i, level j has a level name of "/world_i/level_j"
        identifier : str
            More specifically what the dialogue within the level is. For example, Myla talking to you about a crocodile might be called "myla_croc_talk"
        esacpes : dict of str * str
            A dictionary mapping escape keys (player_name) to its replacement in the game (escapes[player_name] = Tom) to allow parts of the result to be dynamically replaced.

        Returns
        --------
        str
            Returns the specific dialogue requested from the database
        """

        if not(threading.current_thread() in self.conn):
            self.conn[threading.current_thread()] = sqlite3.connect(self.dblocation)
        result = self.conn[threading.current_thread()].execute("SELECT english, french, dutch, hindi, pyrate FROM dialogue WHERE level=? AND identifier=?;", (level_name, identifier))

        row = result.fetchone()
        dialogue = "invalid dialogue identifier" #TODO: Make it so that game complains much more about this in debug mode!!!!
        if(row != None):
            english, french, dutch, hindi, pyrate = row
            if(self.language == "english"):
                dialogue = english
            if(self.language == "french"):
                dialogue = français
            if(self.language == "dutch"):
                dialogue = nederlands
            if(self.language == "hindi"):
                dialogue = hindi
            if(self.language == "pyrate"):
                dialogue = pyrate

        for escape in escapes:
            dialogue = dialogue.replace("%" + escape + "%", escapes[escape])
        return dialogue

    def __init__(self, cpp_engine):
        """ On initialisation, the cplusplus engine is passed to this class to enable it to access the api of the game.

        Parameters
        ----------
        cpp_engine : C++GameEngine
            Represents an instance of the C++ engein. All of whom's properties are inherited by Engine
        """

        self.__cpp_engine = cpp_engine
        self.__game_objects.clear() #Have to do this otherwise the engine seems to still have the objects of the last engine instance
        #Use some magic trickery to give the Engine class all the methods of the C++GameEngine with their functionality
        engine_properties = [a for a in dir(self.__cpp_engine) if not a.startswith('__')]   #get all the engine properties with the magic and private methods filtered out
        for engine_property in engine_properties:                                           #loop over all the engine properties
            if not hasattr(self, engine_property):                                          #only add the property if the engine doesn't have something by that name
                setattr(self, engine_property, getattr(self.__cpp_engine, engine_property)) #set the all the properties of Engine to match cpp_engine.

        #Database
        #--------
        self.all_languages = ["english", "français", "nederlands", "hindi", "pyrate"]
        self.dblocation = os.path.dirname(os.path.realpath(__file__)) + "/../database.db"
        self.language = self.get_config()['game_settings']['language']
        self.conn[threading.current_thread()] = sqlite3.connect(self.dblocation)

    def __del__(self):
        """ Destructor for the game engine.

        On termination all links to the database are closed
        """
        for key in self.conn:
            self.conn[key].close()

    def set_language(self, language_to_set):
        """ Sets the language of the game. (English, French, Dutch, Hindi, Pyrate)
        When querying from the database, it will return the requested string associated with the current language of the game.

        Parameters
        ---------
        language_to_set : str
            The language desired to be set. Currently supported are "english", "français", "nederlands", "hindi", and "pyrate"
        """

        if(language_to_set in self.all_languages):
            self.language = language_to_set
        else:
            print("Not a valid language!")

    def get_language(self):
        """Returns the language the game engine is currently set to

        Returns
        -------
        str
            The string name of the language the game is currently set to
        """


        return self.language

    def register_game_object(self, game_object):
        """ Register a game object.

        Parmeters
        ---------
        game_object : GameObject
            The game object you wish to register
        """
        self.__game_objects[game_object.get_id()] = game_object # associate each game_object with it's id

    def get_objects_at(self, position):
        """ Returns a list of all the objects at a given position

        Overrides the get_objects_at method inherited from cpp engine, as that version returns a list of object ids and this returns
        the actual instances.

        Parameters
        ----------
        position : 2-tuple of int
            The position you want to check for the objects. (x, y)

        Returns
        -------
        list of GameObject
            returns a list of the objects at the given position
        """
        x, y = position                                     #Extract the position x and y coordinates
        game_objects= list()                        #initialise the list that will be returned giving the objects at the position
        object_ids = self.__cpp_engine.get_objects_at(x, y) #get a list of the ids of all the objects at the given position from the game engine
        for object_id in object_ids:                        #iterate over all the object_ids and grab the object associated with each one.
            game_objects.append(self.__game_objects[object_id])

        #return a list of the objects at the given position
        return game_objects

    def is_solid(self, position):
        """ Returns if a given position "is solid" (true if it can't be walked on, false otherwise)

        The Goal of this little wrapper is to make it so that a tuple is accepted.

        Parameters
        ----------
        position: 2-tuple of int
            The position you want to check for solidity. (x,y)

        Returns
        -------
        bool
            Boolean value regarding if the position is solid or not.
        """
        x, y = position                                     #Extract the position x and y coordinates
        return self.__cpp_engine.is_solid(x, y)

    def print_terminal(self, message, highlighted = False, callback = lambda: None):
        """ print the given message to in-game terminal

        Parameters
        ----------
        message
            the message that you wish to print (python will attempt to convert it to a string before printing if possible using the str() method)
        highlighted : bool
            if true the message is printed red, else it printed in black
        """
        self.__cpp_engine.print_terminal(str(message), highlighted)
        self.add_event(callback)

        
    def print_debug(self, message):
        """ print the given message to the operating system terminal

        Parameters
        ----------
        message
            the message that you wish to print (python will attempt to convert it to a string before printing if possible using the str() method)
        """
        self.__cpp_engine.print_debug(str(message))

    def create_object(self, object_file_location, object_name, position):
        """ This is meant to return a new instance of a given game object, but it hasn't been properly implemented yet """
        x, y = position
        entity = self.__cpp_engine.create_object( object_file_location, object_name, x, y)
        return self.wrap_entity_in_game_object(self, entity) #TODO: COMMENT THIS

    def get_tile_type(self, position):
        """Returns an integer corresponding to a specific tile type specified in the SpecialLayer of tiled at the current position.

        The currently special tiles are STANDARD, WATER, SOLID, and QUICKSAND

        Parameters
        ----------
        position: 2-tuple of int
            The position you want to get the tile type of. (x,y)

        Returns
        -------
        int
            returns one of engine.TILE_TYPE STANDARD, engine.TILE_TYPE_WATER, engine.TILE_TYPE_SOLID, or engine.TILE_TYPE_QUICKSAND, corresponding to the values 0,1,2,3.
        """
        x, y = position
        return self.__cpp_engine.get_tile_type(x, y)

    def play_music(self, track_location, callback = lambda: None):
        """ Sets the currently playing music to the given track, this track loops until another one is set

        Parameters
        ----------
        track_location : string
            The name of the track to be played

        callback : func, optional
            Places the callback onto the engine

        """
        self.__cpp_engine.play_music(track_location, callback)


    def get_config(self):
        """ Parses the current config.jsonnet file as a python json object

        Returns
        -------
        python json object
            Returns the config.jsonnet file parsed as a python json object
        """
        result = self.__cpp_engine.get_config()
        return json.loads(result)

    def __get_json_data(self, force_reread = False):
        """
        Returns
        -------
        python json object
            A json object that contains the data of the player. (Last checkpoint, worlds unlocked, etc.)
        """
        if (not self.__json_data) or force_reread:
            settings_string = ""
            with open(self.get_config()['files']['game_save_location'], encoding="utf8") as save_file:
                settings_string = save_file.read()
            self.__json_data= json.loads(settings_string)
        return self.__json_data

    def __save_json_data(self):
        """ Updates the json save file with the current state of the player """

        save_file_string = ""
        with open(self.get_config()['files']['game_save_location'], encoding="utf8") as save_file:
            save_file_string = save_file.read()

        save_json = json.loads(save_file_string)
        save_json = self.__json_data

        with open(self.get_config()['files']['game_save_location'], 'w', encoding="utf8") as save_file:
            json.dump(save_json, save_file, indent=4, separators=(',', ': '))
        self.refresh_config()
        return

    def get_settings(self):
        """ Return the settings from the save.json file as a python json object

        Returns
        -------
        python json object
            The current settings of the game as a json object.
        """
        return self.__get_json_data()["settings"]

    def save_settings(self, settings):
        """ Saves the game settings with the current settings

        Parameters
        ----------
        settings : python json object
            The settings that we want to save. It is whatever that is return by get_settings
        """ #TODO: add resiliency!!!!


        self.__json_data["settings"] = settings
        self.__save_json_data()
        return

    def get_player_data(self, name):
        """ Returns the player data from the save.json file as a python json object.

        Parameters
        ----------
        name : str
            String of a player name that has been initialised with a save.json entry

        Returns
        -------
            The save file of the player with "name" as its name


        """
        if name in self.__get_json_data()["player_saves"]:
            return self.__get_json_data()["player_saves"][name]
        elif name in self.__get_json_data(force_reread = True)["player_saves"]:
            return self.__get_json_data()["player_saves"][name]
        else:
            return {}

    def save_player_data(self, name, game_save):
        """ Saves the player's save state in the json save file

        Parameters
        ----------
        name : str
            The name of the player we want to same the game_save state into
        game_save : save object


        """
        #TODO: add resiliency!!!!
        self.__json_data["player_saves"][name] = game_save
        self.__save_json_data()
        return

    def save_exists(self, name):
        """ Returns whether or not "name" has a save file in save.json.

        This usually indicates if there is a player called "name" that has played the game and saved.

        Parameters
        ----------
        name : str
            The string of the name that we want to check for existence in the save.json file.
        """
        save_data = self.__get_json_data()
        if name in save_data["player_saves"]:
            return True
        else:
            return False

    def set_ui_colours(self, colour_one, colour_two):
        """ Sets the user interface to a color scheme indicated by colour_one and colour_two.

        Colour_one and colour_two are three-tuples indicating the RGB color value of two extremes of a gradient. This gradient is used across the PyScripter and tabs

        Parameters
        ----------
        colour_one : tuple of int * int * int
            The RGB value of one extreme of the color gradient
        colour_two : tuple of int * int * int
            The RGB value of the other extreme of the color gradient
        """

        r1, g1, b1 = colour_one
        r2, g2, b2, = colour_two
        self.__cpp_engine.set_ui_colours(r1, g1, b1, r2, g2, b2)

    def __snake_to_camelcase(self, snake_string):
        """ converts snake_case to CamelCase

        eg "name_name_name" -> "NameNameName"

        Parameters
        ----------
        snake_string : str
            An input string in snake case that we want to convert to Camel Case

        Returns
        -------
        str:
            A string of the input string converted to Camel Case
        """
        components = snake_string.split('_')
        result = ""
        for component in components:
            result += component.title() #title automatically capitalises the first letter of a string! :D
        return result

    def wrap_entity_in_game_object(self, entity):
        """ Imports the correct files and class for each entity, and then wraps them in the correct game object class that has been written in python.

        Used by bootstrapper.py and self.create_object()
        TODO: add a class check to make sure the objects are of the correct class!, have a degrade gracefully,
        if a child class is poorly defined, throw a warning and then try and create an object as their parent until GameObject is reached,
        if GameObject isn't suitable throw and error and say that somthing is seriously wrong!!!!

        Parmeters
        ---------
        entity : C++Entity
            The entity you wish to wrap.
        """
        #Grabs the object's location in the file system (original data comes from the map's tmx file, eg. characters/enemies/crocodile
        entity_location = entity.get_location()
        #Imports the correct module based on that path name
        sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + "/../objects/" + entity_location)  # Go to the correct folder
        # Then get the name of the file itself (same name as the folder it's in, so can be extracted from the path name, eg crocodile/crocodile.py)
        module_name = entity_location[entity_location.rfind("/") + 1: ]
        module = importlib.import_module(module_name) # Import the module as module

        #Get the class from the module, will be the same as it's file but in UpperCamelCase eg. SuperCrocodile in super_crocodile.py
        wrapper_class = getattr(module, self.__snake_to_camelcase(module_name))
        game_object = wrapper_class()  # create the object
        game_object.set_entity(entity, self)  # initialise it and wrap the entity instance in it
        self.__game_objects[game_object.get_id()] = game_object #Store the object and associate with it's id in the engine's dictionary
        return game_object

    def show_dialogue(self, dialogue, disable_scripting = True, callback = lambda: None):
        """ The engine display the dialogue as a pop-up text window.

        This is usually used for dialogue (NPCs talking to the player) or this is run when interacting with a sign. (The pop-up shows the text on the sign)

        Parameters
        ----------
        dialogue : str
            The string that will be display on the dialogue window
        disable_scripting : bool
            A boolean value that indicates if we want the PyScripter to be disabled or not if the dialogue window is up.
        callback : func, optional
            Places the callback onto the engine

        """
        self.__cpp_engine.show_dialogue(dialogue, disable_scripting, callback)

    def close_external_script_help(self, callback = lambda: None):
        self.__cpp_engine.close_external_script_help(callback)

    def show_external_script_help(self, dialogue, callback = lambda: None):
        self.__cpp_engine.show_external_script_help(dialogue, callback)

    def show_dialogue_with_options(self, dialogue, options, disable_scripting = True):
        """ The engine display the dialogue as a pop-up text window with options.

        This is usually used for dialogue with options (NPCs talking to the player and ask if the player wants to help them or not)

        Parameters
        ----------
        dialogue : str
            The string that will be display on the dialogue window
        options : dict of str * func
            A dictionary that maps strings to callbacks. The keys of the dictionary are displayed as the options the player can select, after which options[selected_key] is run.
        disable_scripting : bool
            A boolean value that indicates if we want the PyScripter to be disabled or not if the dialogue window is up.
        """
        self.__cpp_engine.show_dialogue_with_options(dialogue, disable_scripting, options)

    def run_callback_list_sequence(self, callback_list_sequence, callback = lambda: None):
        """ Run the given list of functions, passing the rest of the list as an argument to the first function so that they are run in sequence.

        Ths is commonly used to display sequence of dialogue and to ensure that the functions run in the order specified in the code

        Parameters
        ----------
        callback_list_sequence : list of func
            A list of functions we want to run in sequence with callback linking together. (e.g. the callback taken by every function in this list should be callback).
            Only functions that accept callbacks may be in this list, except for the last function which doesn't necessarily have to accept a callback.
        callback : func, optional
            Places the callback onto the engine
        """
        if not isinstance(callback_list_sequence, collections.deque): #Convert the list to a deque if it isn't already one
            callback_list_sequence = collections.deque(callback_list_sequence)

        if callback_list_sequence:	 #check if it's still got elements
            function = callback_list_sequence.popleft()
            function(lambda: self.run_callback_list_sequence(callback_list_sequence, callback))
        else:
            self.add_event(callback)
        return


    def hide_bag(self, callback = lambda: None):
        """ Hides the bag icon from the top right of the screen

        Parameters
        ----------
        callback : func, optional
            Places the callback onto the engine
        """
        self.__cpp_engine.hide_bag(callback)

    def unhide_bag(self, callback = lambda: None):
        """ Shows the bag icon from the top right of the screen

        Parameters
        ----------
        callback : func, optional
            Places the callback onto the engine
        """
        self.__cpp_engine.unhide_bag(callback)

    def disable_py_scripter(self, callback = lambda: None):
        """ Disables the PyScritpter

        Called during important text when we don't want the player to be able to edit code in their PyScritpetr. Doesn't do anything if the PyScripter is already disabled.

        Parameters
        ----------
        callback : func, optional
            Places the callback onto the engine
        """
        self.__cpp_engine.disable_py_scripter(callback)

    def enable_py_scripter(self, callback = lambda: None):
        """ Enables the PyScritpter

        Enables the player's PyScritpter, usually called after it has been disabled. Doesn't do anything if the PyScritpter is already enabled.

        Parameters
        ----------
        callback : func, optional
            Places the callback onto the engine
        """

        self.__cpp_engine.enable_py_scripter(callback)

    def hide_py_scripter(self, callback = lambda: None):
        """ Hides the PyScritpter from the player

        Called when we don't want the player to see the PyScritpter (e.g. before they have even received the PyScripter)

        Parameters
        ----------
        callback : func, optional
            Places the callback onto the engine
        """
        self.__cpp_engine.hide_py_scripter(callback)

    def show_py_scripter(self, callback = lambda: None):
        """ Allows the player to see the PyScritpter

        Called when the player first receives the PyScritpter

        Parameters
        ----------
        callback : func, optional
            Places the callback onto the engine
        """
        self.__cpp_engine.show_py_scripter(callback)

    def enable_script_editing(self, callback = lambda: None):
        """ Allows the player to edit code in their PyScripter

        Called after the PyScripter has been disabled and we want to enable it, usually called at the end of a dialogue sequence

        Parameters
        ----------
        callback : func, optional
            Places the callback onto the engine
        """
        self.__cpp_engine.enable_script_editing(callback)

    def disable_script_editing(self, callback = lambda: None):
        """ Does not allow the player to edit code in their PyScripter

        usually called at the beginning of a dialogue sequence and we don't want the player to edit their PyScripter

        Parameters
        ----------
        callback : func, optional
            Places the callback onto the engine
        """
        self.__cpp_engine.disable_script_editing(callback)

    def set_py_tabs(self, num_tabs, callback = lambda: None):
        """ Sets the number of tabs on the PyScripter to num_tabs.

        We incrementally increase the number of tabs in the player's PyScripter as he/she progresses in the game

        Parameters
        ----------
        num_tabs : int
            An integer between 1 and 9 that indicates the number of tabs we want the PyScripter to have.
        callback : func, optional
            Places the callback onto the engine


        """
        self.__cpp_engine.set_py_tabs(num_tabs, callback)

    def show_external_script(self, confirm_callback = lambda: None, cancel_callback = lambda: None, external_dialogue = "", script_init = lambda: None, character_object = None):
        """ This function is used to have the player give a script to an NPC (after which the NPC may or may not run the script)

        When an NPC requires a script he will ask the player if he/she can help write a script. If the player says 'Give script;, confirm_callback is run, if the player says 'Cancel', the cancel_callback is run. The tab that the player is given to edit is
        the external_tab that can be initialized via script_init and external dialogue is displayed in the notification bar when this tab is open.

        There are two cases for this external script:

        Fixing NPC scripts
            - Do not pass a character
            - Just pass the script initialisation that you want
            - The script is not persistent i.e. each time you are given this script it starts with the same initialisation (so you can try and fix it again)

        Writing NPC scripts from scratch
            - Do not pass any script initialisation
            - The script is stored with the character so the next time you work on their script, it restores what you had already done.

        Parameters
        ----------
        confirm_callback : func
            The callback that is run if the player clicks on "Give Script"
        cancel_callback : func
            The callback that is run if the player clicks "Cancel"
        external_dialogue :  str, optional
            String that gets displayed in the dialogue window while the external PyScript is open
        script_init : func, optional
            The callback that is run when tab is created and before the player confirms/cancels. This is used to initialize the external script and commonly includes "insert_to_scripter"
        character_object : character, optional
        """

        #Add function calls to the confirm callback if the script needs storing with the character
        def store_script():
            confirm_callback()
            character_object.set_script(self.get_external_script())

        if character_object:
            script_init = lambda: self.insert_to_scripter(character_object.get_script())
            confirm_and_store_script_callback = store_script
            self.__cpp_engine.show_external_script(confirm_and_store_script_callback, cancel_callback, external_dialogue, script_init)
        else:
            self.__cpp_engine.show_external_script(confirm_callback, cancel_callback, external_dialogue, script_init)

    def insert_to_scripter(self, text, callback = lambda: None):
        """ Inserts code into the PyScripter

        This function can be used to have an NPC give the player code to run.

        Parameters
        ----------
        text : str
            The text/code we wish to insert into the current tab of the PyScripter
        callback : func, optional
            Places the callback onto the engine
        """
        self.__cpp_engine.insert_to_scripter(text)
        self.add_event(callback)

    def clear_scripter(self, callback = lambda: None):
        """ Clears the PyScripter. (Deletes all code associated with the current tab)

        Parameters
        ----------
        callback : func, optional
            Places the callback onto the engine

        """
        self.__cpp_engine.clear_scripter(callback)

    def change_map(self, map_name):
        """ Changes from the current map to next map with file path map_name

        Used to link levels together and "enter buildings" etc.
        On death change_map is called to change to the current map, restarting the level.

        Parameters
        ----------
        map_name : str
            String of the path to the folder containing the level/map we want to change to
        """

        self.__cpp_engine.change_map(map_name)
        raise

    def get_all_objects(self):
        """ Returns all of the game object in the current engine (characters/tiles/etc)

        Returns
        -------
        list of game_object
            A list of all the game_object
        """
        return list(self.__game_objects.values())

    def get_error(self):
        """Gets if the last run of the PyScripter ran with errors or not

        Returns
        -------
        bool
            If the last run of the PyScripter contained an error
        """
        return self.__error

    def set_error(self, has_error):
        """Sets a flag called error regarding if the last run of the PyScripter ran with errors or not

        Parameters
        ----------
        has_error : bool
            If the last run had an error or not
        """
        self.__error = has_error

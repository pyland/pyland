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

    __json_data = None

    def get_dialogue(self, level_name, identifier, escapes = dict()):
        """ Get the piece of dialoge requested form the database.

        The escapes argument is a dictionary from escape keys to replacements to allow parts of the result to be dynamically replaced.
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
        for key in self.conn:
            self.conn[key].close()

    def set_language(self, language_to_set):
        if(language_to_set in self.all_languages):
            self.language = language_to_set
        else:
            print("Not a valid language!")

    def get_language(self):
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

        The Goald if this little wrapper is to make it so that a tuple is accepted.
        """
        x, y = position                                     #Extract the position x and y coordinates
        return self.__cpp_engine.is_solid(x, y)

    def print_terminal(self, message, highlighted = False):
        """ print the given message to in-game terminal

        Parameters
        ----------
        message
            the message that you wish to print (python will attempt to convert it to a string before printing if possible using the str() method)
        highlighted : bool
            if true the message is printed red, else it printed in black
        """
        self.__cpp_engine.print_terminal(str(message), highlighted)

    def create_object(self, object_file_location, object_name, position):
        """ This is meant to return a new instance of a given game object, but it hasn't been properly implemented yet """
        x, y = position
        entity = self.__cpp_engine.create_object( object_file_location, object_name, x, y)
        return self.wrap_entity_in_game_object(self, entity)

    def get_tile_type(self, position):
        x, y = position
        return self.__cpp_engine.get_tile_type(x, y)

    def open_dialogue_box(self, callback = lambda: None):
        self.__cpp_engine.open_dialogue_box(callback)

    def get_config(self):
        """ Return the config.jsonnet file parsed as a python json object """
        result = self.__cpp_engine.get_config()
        return json.loads(result)

    def __get_json_data(self):
        if not self.__json_data:
            settings_string = ""
            with open(self.get_config()['files']['game_save_location'], encoding="utf8") as save_file:
                settings_string = save_file.read()
            self.__json_data= json.loads(settings_string)
        return self.__json_data

    def __save_json_data(self):
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
        "Return the settings from the save.json file as a python json object """
        return self.__get_json_data()["settings"]

    def save_settings(self, settings):
        """save the game settings""" #TODO: add resiliency!!!!
        self.__json_data["settings"] = settings
        self.__save_json_data()
        return

    def get_player_data(self, name):
        "get the player's save"
        return self.__get_json_data()["player_saves"][name]

    def save_player_data(self, name, game_save):
        """save the player's save""" #TODO: add resiliency!!!!
        self.__json_data["player_saves"][name] = game_save
        self.__save_json_data()
        return

    def save_exists(self, name):
        save_data = self.__get_json_data()
        if name in save_data["player_saves"]:
            return True
        else:
            return False

    def set_ui_colours(self, colour_one, colour_two):
        r1, g1, b1 = colour_one
        r2, g2, b2, = colour_two
        self.__cpp_engine.set_ui_colours(r1, g1, b1, r2, g2, b2)

    def __snake_to_camelcase(self, snake_string):
        """ converts snake_case to CamelCase

        eg "name_name_name" -> "NameNameName"
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
        self.__cpp_engine.show_dialogue(dialogue, disable_scripting, callback)

    def show_dialogue_with_options(self, dialogue, disable_scripting = True, callback = lambda: None):
        self.__cpp_engine.show_dialogue_with_options(dialogue, disable_scripting, callback)

    def run_callback_list_sequence(self, callback_list_sequence, callback = lambda: None):
        """ Run the given list of functions, passing the rest of the list as an argument to the first function so that they are run in sequence.
        """
        if not isinstance(callback_list_sequence, collections.deque): #Convert the list to a deque if it isn't already one
            callback_list_sequence = collections.deque(callback_list_sequence)

        if callback_list_sequence:	 #check if it's still got elements
            function = callback_list_sequence.popleft()
            function(lambda: self.run_callback_list_sequence(callback_list_sequence, callback))
        else:
            self.add_event(callback)
        return


    def disable_py_scripter(self, callback = lambda: None):
        self.__cpp_engine.disable_py_scripter(callback)

    def enable_py_scripter(self, callback = lambda: None):
        self.__cpp_engine.enable_py_scripter(callback)

    def enable_script_editing(self, callback = lambda: None):
        self.__cpp_engine.enable_script_editing(callback)

    def disable_script_editing(self, callback = lambda: None):
        self.__cpp_engine.disable_script_editing(callback)

    def set_py_tabs(self, num, callback = lambda: None):
        self.__cpp_engine.set_py_tabs(num, callback)

    def show_external_tab(self, confirm_callback = lambda: None, cancel_callback = lambda: None, external_dialogue = False, callback = lambda: None):
        self.__cpp_engine.show_external_tab(confirm_callback, cancel_callback, external_dialogue, callback)

    def hide_external_tab(self, callback = lambda: None):
        self.__cpp_engine.hide_external_tab(callback)

    def insert_to_scripter(self, text, callback = lambda: None):
        self.__cpp_engine.insert_to_scripter(text)
        self.add_event(callback)

    def clear_scripter(self, callback = lambda: None):
        self.__cpp_engine.clear_scripter(callback)

    def change_map(self, map_name):
        self.__cpp_engine.change_map(map_name)
        raise


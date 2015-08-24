import operator
import os

import sys
sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/..')
from game_object import GameObject

import json

#TODO: add checks in all of these functions so that states can be lazily made etc.

""" This class is a useful object which is useful in that automatically handels a lot of the 
saving that the Pyland game requires. 


All the save stuff from engine needs to be abstracted out to here as ultimately this will off a more robust and flexible system

"""
class PlayerData(GameObject):


    __current_world = None
    __current_level = None
    __current_map = None


    __player_name = None

    __player_data = None
    __level_data = None

    def initialise(self):
        pass

    def create(self, player_name):
        """ create a new save-game with the name given """
        engine = self.get_engine()
        save_string = ""
        with open(engine.get_config()['files']['initial_save_location'], encoding="utf8") as initial_save_file:
            save_string = initial_save_file.read()
        json_data = json.loads(save_string)
        engine.save_player_data(player_name, json_data)
        return

    def load(self, player_name):
        """ loads in the player data, and correctly set's up all the global states of the game.

        For example, it displays the correct number of totems, PyScipter tabs etc. """
        engine = self.get_engine()
        self.__player_name = player_name
        self.__player_data = engine.get_player_data(player_name)
        return

    def save(self, ):
        self.get_engine().save_player_data(self.__player_name, self.__player_data)
        return

    def set_map(self, world_name, level_name = None, map_name = None):
        """ set the current map of the game. This is done when every map is loading so that
        the latest player map-state will always be saved """
        self.__current_world = world_name
        self.__current_level = level_name
        self.__current_map = map_name

        self.__player_data["current_map"] = self.get_full_map_name()
        self.save()
        return

    def get_full_map_name(self):
        full_map_name = "/" + self.__current_world
        if self.__current_level != None:
            full_map_name += "/" + self.__current_level
        if self.__current_map != None:
            full_map_name += "/" + self.__current_map
        return full_map_name
        

    def get_previous_exit(self):
        return self.__player_data["previous_exit"]

    def save_and_exit(self, destination, info = None):
        """ Go to the given destination and save the game """
        previous_exit = self.get_full_map_name()
        if(info != None):
            previous_exit += "/" + info
        self.__player_data["previous_exit"] = previous_exit
        self.save()
        self.get_engine().change_map(destination)

    def __get_level_data(self):
        return self.__player_data["level_data"][self.__current_world][self.__current_level]

    def get_level_state(self, level_state):
        """ Get some kind of state associated with the level, the default value is 0, otherwise it returns the number saved there. """
        level_data = self.__get_level_data()
        return level_data[level_state]

    def set_level_state(self, level_state, value):
        #TODO: add checks to this so that if the state doesn't exist it is created
        self.__get_level_data()[level_state] = value
    
            

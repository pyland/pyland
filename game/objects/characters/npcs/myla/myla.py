import sys
import os
sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/../../../characters')
#sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/../../../game_object')
#from game_object import GameObject
from character import Character #TODO: it should import from here, but some kind of definitions with the movement functions are screwing around with follow_path, causing the game to freeze.

import random

""" A brief description of the class will go here.
The auto-generated comment produced by the script should also mention where to get a list of the api
and which built-in variables exist already.
"""
class Myla(Character):

    def initialise(self):
        super().initialise()

    def player_action(self, player_object):
        """ This is the method that is run if the player presses the action key while facing a character.
        
        TODO: move this to character.py and make it print an explenation on how to override this by default.
        """
        pass







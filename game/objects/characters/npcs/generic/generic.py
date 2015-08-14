import sys
import os

sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/../../../characters')
from character import Character

""" A brief description of the class will go here.
The auto-generated comment produced by the script should also mention where to get a list of the api
and which built-in variables exist already.
"""
class Generic(Character):

    def initialise(self):
        super().initialise()

    def player_action(self, player_object):
        pass







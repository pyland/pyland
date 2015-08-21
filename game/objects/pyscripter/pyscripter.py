import operator
import os
import sys

sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/..')
from game_object import GameObject

class Pyscripter(GameObject):

    def initialise(self):
        self.set_sprite("")
        self.set_visible(True)
        self.set_solidity(True)

    def player_action(self, player_object):
        engine = self.get_engine()
        player_object.set_busy(True)
        engine.show_dialogue("I am the PyScripter", callback = lambda: player_object.set_busy(False))

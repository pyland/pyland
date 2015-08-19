import operator
import os

import sys
sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/..')
from game_object import GameObject


class TileTrigger(GameObject):
    def __init__(self):
        self.num_walk = 0

    def player_walked_on(self, player_object):
        self.get_engine().print_terminal("I've just been walked on by: " + player_object.get_character_name())
        self.get_engine().print_terminal(self.num_walk)

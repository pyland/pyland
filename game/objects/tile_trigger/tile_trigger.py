import operator
import os

import sys
sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/../game_object')
from game_object import GameObject


class TileTrigger(GameObject):
    __target = (0, 0)

    def initialise(self):
        self.set_sprite("")
        self.set_visible(True)


    def player_walked_on(self, player_object):
        self.get_engine().print_terminal("I've just been walked on by: " + player_object.get_character_name())
        position = self.__target
        player_object.move_to(position)

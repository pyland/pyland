import operator
import os

import sys
sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/..')
from game_object import GameObject


class Totem(GameObject):

    __totem_number = 0
    __totem_text = ""

    def initialise(self):
        self.set_visible(True)
        self.set_solidity(True)
        self.set_sprite("")
        self.__totem_number = int(self.get_name()[6:]) #get the totem number, as all totems are called totem_n, the totem number is it's name minus the first 6 characters
        self.__totem_text = self.get_engine().get_dialogue("totems", self.__totem_number)
        
    def player_action(self, player_object):
        engine = self.get_engine()
        player_data = engine.get_object_called("player_data")
        engine.run_callback_list_sequence([
            lambda callback: player_object.set_busy(True, callback = callback),
            lambda callback: engine.show_dialogue(self.__totem_text, callback = callback),
            lambda callback: player_data.unlock_totem(self.__totem_number, callback = callback),
            lambda callback: player_object.set_busy(False, callback = callback),
        ])

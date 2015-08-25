import operator
import os
import sys
sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/..')
from game_object import GameObject

class Whetstone(GameObject):

    __replenish_power = 0
    __readied = False
    __vine_list = []

    def initialise(self):
        self.set_sprite("")
        self.set_visible(True)
        self.set_solidity(True)

    def player_action(self, player_object):
        engine = self.get_engine()
        player_object.set_busy(True)
        self.__sharpen_knife(player_object)

    def __sharpen_knife(self, player_object):
        #TODO: regrow all the vines
        player_object.set_busy(True)
        if(self.__replenish_power > player_object.get_cuts_left()):
            player_object.set_cuts_left(self.__replenish_power)
        for vine in self.__vine_list:
            vine.restore()
        self.get_engine().run_callback_list_sequence([
            lambda callback: self.get_engine().show_dialogue("You used the whetstone, to replenished your knife to have " + str(self.__replenish_power) + " cut(s)", callback = callback),
            lambda callback: self.get_engine().show_dialogue("All the vines in the area have been restored.", callback = callback),
            lambda callback: player_object.set_busy(False, callback = callback)
        ])

    def prepare(self, replenish_power, vine_list):
        self.__readied = True
        self.__vine_list = vine_list
        self.__replenish_power = replenish_power

import operator
import os
"""
In Python comments,
could define some standard which the C++ code can use to determine things about it handles 
the python code
"""

"""
eg. We could be able to write:
"""
#__ import_object  game_object/GameObject
"""
The game code, at runtime, could recognise the "#__"
and replace it with:
"""
import sys
sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/..')
from game_object import GameObject
"""
As the GameObject is in the base objects folder.
"""


"""
"""
class Knife(GameObject):

    __message = "Looks like a sharp knife!"
    __cuts_left = 0

    def initialise(self):
        self.set_sprite("")
        self.set_visible(True)
        self.set_solidity(False)

    def player_action(self, player_object):
        if not self.is_visible():
            return
        engine = self.get_engine()
        player_object.set_busy(True)
        engine.show_dialogue(self.__message, callback = lambda : player_object.set_busy(False))

    def player_walked_on(self, player_object):
        #self.get_engine().print_terminal("Been Walked on")
        if(self.is_visible()):
            self.contact_action(player_object)

    def contact_action(self, player_object):
        player_object.set_busy(True)
        self.get_engine().run_callback_list_sequence([
            lambda callback: self.get_engine().show_dialogue("Picked up knife!", callback = callback), 
            lambda callback: self.set_visible(False, callback = callback),
            lambda callback: player_object.set_busy(False, callback = callback)
        ])

    def set_cuts_left(self, cuts_left):
        self.__cuts_left = cuts_left

    def cut(self):
        self.__cuts_left = self.__cuts_left - 1
        engine.print_terminal("Swoosh! This knife can now cut " + str(self._cuts_left) + "times")
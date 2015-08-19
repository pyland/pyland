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
class Sign(GameObject):

    __message = "You need to set the message on this sign!"

    def initialise(self):
        self.set_sprite("")
        self.set_visible(True)
        self.set_solidity(True)

    def player_action(self, player_object):
        engine = self.get_engine()
        player_object.set_busy(True)
        engine.show_dialogue(self.__message, lambda: player_object.set_busy(False))
    
    def set_message(self, message):
        self.__message = message

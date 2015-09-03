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
class Scroll(GameObject):

    __message = ""

    def initialise(self):
        self.set_sprite("")
        self.set_visible(True)
        self.set_solidity(True)

    def on_scan(self, callback = lambda: None):
        return self.__message

    def player_action(self, player_object):
        if not self.is_visible():
            return
        else:
            player_object.set_busy(True, callback = lambda: self.get_engine().show_dialogue("Looks like a scroll. A PyScripter could scan the message on it...", callback = lambda: player_object.set_busy(False)))

    def set_message(self, message):
        self.__message = message

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
class Vine(GameObject):

    def initialise(self):
        self.grow()

    def on_cut(self):
        self.set_visible(False)
        self.set_solidity(False)
        return True

    def grow(self):
        self.set_visible(True)
        self.set_solidity(True)
        self.start_animating(loop = False, speed = 0.08)

    def restore(self):
        """ Restore the vine to its starting state, used by the whetstone."""
        self.grow()

    def player_action(self, player_object):
        player_object.set_busy(True, callback = lambda: self.get_engine().show_dialogue("Vines can be removed but cutting them...", callback = lambda: player_object.set_busy(False)))

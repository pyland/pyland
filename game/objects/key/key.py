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
class Key(GameObject):

    __message = "Looks like a key!"
    __key_value = 1

    def initialise(self):
        self.set_sprite("")
        self.set_visible(True)
        self.set_solidity(True)

    def player_action(self, player_object):
        if not self.is_visible():
            return
        engine = self.get_engine()
        player_object.set_busy(True)
        player_object.add_keys(self.__key_value)
        engine.run_callback_list_sequence([
            lambda callback: engine.show_dialogue("Picked up the key! You now have " + str(player_object.get_keys()) + " key(s)", callback = callback), 
            lambda callback: player_object.set_busy(False, callback = callback),
            lambda callback: self.set_visible(False, callback = callback),
            lambda callback: self.set_solidity(False, callback = callback)
        ])

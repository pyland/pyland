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
from vine import Vine
"""
As the GameObject is in the base objects folder.
"""


class PoisonVine(Vine):
    """ The poison vine is a special kind of vine that can kill the player

    """

    def _passive_grow(self, callback = lambda: None):
        def un_solidify():
            self.set_solidity(False)
            self.get_engine().add_event(callback)
        super()._passive_grow(callback = un_solidify)

    def player_action(self, player_object):#if not self.__visible:
        if not self.is_visible():
            return
        else:
            player_object.set_busy(True, callback = lambda: self.get_engine().show_dialogue("These vines look poisonous, better not touch them.", callback = lambda: player_object.set_busy(False)))

    def player_walked_on(self, player_object):
        if(self.is_visible() and self._is_alive()):
            self._kill_player(player_object)



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

    def initialise(self):
        super().initialise()
        self.set_solidity(False)

    def player_action(self, player_object):
        player_object.set_busy(True, callback = lambda: self.get_engine().show_dialogue("These vines look poisonous, better not touch them.", callback = lambda: player_object.set_busy(False)))

    def player_walked_on(self, player_object):
        #self.get_engine().print_terminal("Been Walked on")
        if(not self._is_alive()):
            return
        if(self.is_visible()):
            self.contact_action(player_object)

    def contact_action(self, player_object):
        if(not self._is_alive()):
            return
        if(not self.is_visible()):
            return
        self.get_engine().run_callback_list_sequence([
            lambda callback: player_object.set_busy(True, callback = callback),
            lambda callback: self.get_engine().show_dialogue("Aagh! The vines got you!", callback = callback),
            lambda callback: player_object.set_busy(False, callback = callback)
        ])



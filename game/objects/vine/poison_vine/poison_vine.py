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

    __message = "Looks like a spiky and poisonous vine! Better avoid it."
    __alive = False

    def initialise(self):
        self.set_sprite("")
        self.__alive = True
        self.set_visible(True)
        self.set_solidity(True)

    def grow(self, player_object, callback = lambda : None):
        if(not self.__alive):
            return
        if(self.get_position() == player_object.get_position()):
            self.contact_action()
        self.set_visible(True)
        self.start_animating(speed = 0.1, loop = False, forward = True, callback = callback)

    ##temporarily destroy a vine - this is not the same as killing it
    def destroy(self, callback = lambda: None):
        if(not self.__alive):
            return
        self.start_animating(speed = 0.1, loop = False, forward = False, callback = lambda: self.set_visible(False, lambda : callback))
        callback()

    def kill(self):
        if(self.__alive):
            self.stop_animating()
            self.set_solidity(False)
            self.set_sprite("dead")
            self.__alive = False
            return True

    def player_action(self, player_object):
        if not self.is_visible():
            return
        engine = self.get_engine()
        player_object.set_busy(True)
        engine.show_dialogue(self.__message, callback = lambda: player_object.set_busy(False))

    def player_walked_on(self, player_object):
        #self.get_engine().print_terminal("Been Walked on")
        if(not self.__alive):
            return
        if(self.is_visible()):
            self.contact_action()

    def on_cut(self):
        return self.kill()

    def contact_action(self):
        if(not self.__alive):
            return
        self.get_engine().print_terminal("Oh no! The vines got you!")

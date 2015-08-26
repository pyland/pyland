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
    __alive = True

    def initialise(self):
        self._passive_grow()

    def on_cut(self):
        if self.__alive = False:
            return False
        self.__alive = False
        self.start_animating(loop = False, forward = False, speed = 0.1, callback = lambda: self.set_visible(False, callback = 	self.set_solidity(False)))
        return True

    def _kill_player(self, player_object):
        self.get_engine().run_callback_list_sequence([
            lambda callback: player_object.set_busy(True, callback = callback),
            lambda callback: self.get_engine().show_dialogue("Aagh! The vines got you!", callback = callback),
            lambda callback: player_object.set_busy(False, callback = callback)
        ], callback = player_object.kill)

    def grow(self, callback = lambda: None):
        def check_for_and_kill_player():
            objects = self.get_engine().get_objects_at(self.get_position())
            for game_object in objects:
                if hasattr(game_object, "kill"):
                    self._kill_player(game_object)
                    return
            self.get_engine().add_event(callback)
        self._passive_grow(callback = check_for_and_kill_player)

    def _passive_grow(self, callback = lambda: None):
        if self.__alive:
            self.set_visible(True)
            self.set_solidity(True)
            self.start_animating(loop = False, speed = 0.1, callback = callback)

    def restore(self):
        """ Restore the vine to its starting state, used by the whetstone."""
        self.__alive = True
        self.grow()

    def player_action(self, player_object):
        if not self.is_visible():
            return
        else:
            player_object.set_busy(True, callback = lambda: self.get_engine().show_dialogue("Vines can be removed but cutting them...", callback = lambda: player_object.set_busy(False)))

    def _is_alive(self):
        return self.__alive

    def shrink(self, callback = lambda: None):
        if not self.__alive:
            return
        self.start_animating(speed = 0.1, loop = False, forward = False, callback = lambda: self.set_visible(False, callback = lambda: self.set_solidity(False, callback = callback)))



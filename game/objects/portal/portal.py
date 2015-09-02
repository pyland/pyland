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
class Portal(GameObject):
    __target = (0, 0)

    def initialise(self):
        self.set_sprite("")
        self.set_visible(True)

    def set_target(self, target):
        """ Set the target for the portal, it can be either another portal or a tuple of integers """
        if(isinstance(target, Portal)):
            self.__target = target.get_position()
        else:
            self.__target = target

    def player_walked_on(self, player_object):
        #self.get_engine().print_terminal("Portal has just been walked on by: " + player_object.get_character_name())
        position = self.__target
        player_object.move_to(position)

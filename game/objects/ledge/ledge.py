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

class Ledge(GameObject):

    __direction = 0
    def __move_north():
        pass
    def __move_east():
        pass
    def __move_west():
        pass
    def __move_south():
        pass

    def initialise(self):
        self.set_sprite("")
        self.set_visible(True)
        self.set_solidity(False)

    #Here, direction indicates the direction of "uphill" for the ledge
    #For instance, if direction is north, you can jump down the ledge towards the south, but not climb up north
    #North is 1, East is 2, West is 3, South is 4
    #Tis like NEWS
    def set_ledge_face(self, direction):
        self.__direction = direction

        if self.__direction == 1:
            self.set_sprite("north")

        elif self.__direction == 2:
            self.set_sprite("east")

        elif self.__direction == 3:
            self.set_sprite("west")

        elif self.__direction == 4:
            self.set_sprite("south")

    def player_walked_on(self, player_object):
        
        if not player_object.is_solid():
            return

        if self.__direction == 1:
            self.__move_north = player_object.move_north

            def block_north(callback = lambda: None):
                player_object.face_north()
                player_object.move_on_spot(callback)

            player_object.move_north = block_north

        elif self.__direction == 2:
            self.__move_east = player_object.move_east

            def block_east(callback = lambda: None):
                player_object.face_east()
                player_object.move_on_spot(callback)

            player_object.move_east = block_east

        elif self.__direction == 3:
            self.__move_west = player_object.move_west

            def block_west(callback = lambda: None):
                player_object.face_west()
                player_object.move_on_spot(callback)

            player_object.move_west = block_west

        elif self.__direction == 4:
            self.__move_south = player_object.move_south

            def block_south(callback = lambda: None):
                player_object.face_south()
                player_object.move_on_spot(callback)

            player_object.move_south = block_south


    def player_walked_off(self, player_object):

        if not player_object.is_solid():
            return

        if self.__direction == 1:
            player_object.move_north = self.__move_north

        elif self.__direction == 2:
            player_object.move_east = self.__move_east

        elif self.__direction == 3:
            player_object.move_west = self.__move_west

        elif self.__direction == 4:
            player_object.move_south = self.__move_south


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
sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/../game_object')
from game_object import GameObject
"""
As the GameObject is in the base objects folder.
"""


""" This class is the parent of all characters in the game
It provides a lot of useful prebuilt functionality, mainly to do with movement animation.

Lets say you have an object called 'x' in objects/x, by default a sprite will be looked for in objects/x/sprites/main.png, if none is found,
the object will be invisible in game.

However, a character MUST have at least the following sprites in the specified (relative to objects/x/sprites) locations.

If the sprite location is changed, the following relative paths to that location must still be valid sprites.
main/north/1.png (state/direction_facing/animation_frame.png)
main/north/2.png
main/north/3.png
main/north/4.png

main/east/1.png
main/east/2.png
main/east/3.png
main/east/4.png

main/south/1.png
main/south/2.png
main/south/3.png
main/south/4.png

main/west/1.png
main/west/2.png
main/west/3.png
main/west/4.png


Here the first folder represents the state of the character. (All characters MUST have a "main" state)
The second folder represents the direction they are facing.
The filename represents the frames of animation of the character when they are moving. (The first frame is implicitly assumed to be used when they are still)

This benifits of this class is that it provides very simple methods for movement + animation.
However, it doesn't check if the tiles that are being moved to are empty or not. TODO: Talk about maybe changing this?
"""
class Map:
    def solid_objects_at(self, position):
        (x, y, z) = position
        return []

game_map = Map()

class Character(GameObject):

    def __init(self, name):
        super().__init__(name)
        self.set_sprite("main/north")

    """ Returns True if the character is moving,
    False otherwise.
    """
    def moving(self):
        return self.__moving
    
    """ Change the sprite folder to "north" """
    def face_north(self):
        self.__face("north")
        return
    
    """ Change the sprite folder to "east" """
    def face_east(self):
        self.__face("east")
        return

    """ Change the sprite folder to "south" """
    def face_south(self):
        self.__face("south")
        return

    """ Change the sprite folder to "west" """
    def face_west(self):
        self.__face("west")
        return

    """ Get the character to "face" the direction specified
    simply changes the last part of the sprite folder as relevant
    """
    def __face(self, direction):
        sprite_location = self.get_sprite()
        sprite_location = sprite_location[0 : sprite_location.rfind("/")] #slice all the characters after the last "/" from the string
        self.set_sprite(sprite_location + direction) #sprites are now looked for in direction folder :)
        return

    """ Says if the character is facing north """
    def is_facing_north(self):
        return __is_facing("north")

    """ Says if the character is facing east """
    def is_facing_east(self):
        return __is_facing("east")

    """ Says if the character is facing south """
    def is_facing_south(self):
        return __is_facing("south")

    """ Says if the character is facing west """
    def is_facing_west(self):
        return __is_facing("west")

    """ Checks to see if the character is facing in the direction given
    direction -- a string specifing the direction, either 'north', 'east', 'south' or 'west'
    """
    def __is_facing(self, direction):
        sprite_location = self.get_sprite()  # The direction is related to where the sprite is stored, so extract the information from there
        sprite_location = sprite_location[sprite_location.rfind("/") : ] # Slice all the characters before the last "/" from the string
        return (sprite_location == direction)
    
    """ Moves the character in the given direction by one tile and animates them
    face_x -- the function to make the character face in the correct direction
    parent_move_x -- the parent move function, calls the engine api but doesn't start the animation
    callback -- the function that you would like to call after the movement is complete
    """
    def __move_x(self, face_x, parent_move_x, callback):
        face_x()
        self.start_animating()
        def callbacktwo():  # Have create a wrapper callback function which makes sure that the animation stops before anything else is run
            self.stop_animating()
            callback()
        parent_move_x(callbacktwo)
        return
    
    """ Moves the character North by one tile and makes them face in that direction
    callback -- the function that you would like to call after the movement is complete
    """
    def move_north(self, callback = lambda: None):
        (x, y, z) = self.get_position()
        if(len(game_map.solid_objects_at((x, y + 1, z))) == 0): #check that the relevant location is free
            self.__move_x(self.face_north, super().move_north, callback)
        return

    """ Move character in direction by one tile. 
    Overides general object implementation
    """
    def move_east(self, callback = lambda: None):
        (x, y, z) = self.get_position()
        if(len(game_map.solid_objects_at((x + 1, y, z))) == 0): #check that the relevant location is free
            self.__move_x(self.face_east, super().move_east, callback)
        return
    
    """ Move character in direction by one tile. 
    Overides general object implementation
    """
    def move_south(self, callback = lambda: None):
        (x, y, z) = self.get_position()
        if(len(game_map.solid_objects_at((x, y - 1, z))) == 0): #check that the relevant location is free
            self.__move_x(self.face_south, super().move_south, callback)
        return

    """ Move character in direction by one tile. 
    Overides general object implementation
    """
    def move_west(self, callback = lambda: None):
        (x, y, z) = self.get_position()
        if(len(game_map.solid_objects_at((x - 1, y, z))) == 0): #check that the relevant location is free
            self.__move_x(self.face_west, super().move_west, callback)
        return

    def get_facing(self):
        return #parse sprite_location to get facing

    def before_frame_update(self):
        super().before_frame_update()
        self.__handle_movement_input()

    def change_state(self, state):
        if(isinstance(state, str)):
            self.__state = state
        return
    

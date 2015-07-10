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
class Character(GameObject):

    def __init(self):
        super().__init__()
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
    """
    def __is_facing(self, direction):
        sprite_location = self.get_sprite()
        sprite_location = sprite_location[sprite_location.rfind("/") : ] #slice all the characters before the last "/" from the string
        return (sprite_location == direction)
        
    """ Move character in direction by one tile. 
    Overides general object implementation
    """
    def move_north(self, callback):
        (x, y, z) = self.get_position()
        if(api.solid_objects_at((x, y + 1, z)).length == 0): #check that the relevant location is free
            self.face_north()
            self.start_animating()
            
            def callbacktwo:
                self.stop_animating()
                callback() 
            
            super().move_north(callbacktwo) #pass the callback to say you want the object to stop animating when the operation is complete
        return

    
    def move_north(self):
        self.move_north(lambda: pass) #move without the callback

    """ Move character in direction by one tile. 
    Overides general object implementation
    """
    def move_east(self, callback):
        (x, y, z) = self.get_position()
        if(api.solid_objects_at((x + 1, y, z)).length == 0): #check that the relevant location is free
            self.face_east()
            self.start_animating()
            
            def callbacktwo:
                self.stop_animating()
                callback() 
            
            super().move_east(callbacktwo) #pass the callback to say you want the object to stop animating when the operation is complete
        return

    def move_east(self):
        self.move_east(lambda: pass) #move without the callback
    
    """ Move character in direction by one tile. 
    Overides general object implementation
    """
    def move_south(self, callback):
        (x, y, z) = self.get_position()
        if(api.solid_objects_at((x, y - 1, z)).length == 0): #check that the relevant location is free
            self.face_south()
            self.start_animating()
            
            def callbacktwo:
                self.stop_animating()
                callback() 
            
            super().move_south(callbacktwo) #pass the callback to say you want the object to stop animating when the operation is complete
        return

    def move_south(self):
        self.move_south(lambda: pass) #move without the callback

    """ Move character in direction by one tile. 
    Overides general object implementation
    """
    def move_west(self, callback):
        (x, y, z) = self.get_position()
        if(api.solid_objects_at((x - 1, y, z)).length == 0): #check that the relevant location is free
            self.face_west()
            self.start_animating()
            
            def callbacktwo:
                self.stop_animating()
                callback() 
            
            super().move_west(callbacktwo) #pass the callback to say you want the object to stop animating when the operation is complete
        return

    def move_west(self):
        self.move_west(lambda: pass) #move without the callback

    def get_facing(self):
        return #parse sprite_location to get facing

    def before_frame_update(self):
        super().before_frame_update()
        self.__handle_movement_input()

    def change_state(self, state):
        if(isinstance(state, str)):
            self.__state = state
        return
    

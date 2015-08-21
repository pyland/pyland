import operator
import os
from random import randint
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

    __character_name = ""

    __busy = False #A business flag used in some circumstances to check if the character is already occupied with something else to make sure certain things don't clash

    __finish_turning = False#A flag to specify whether the character is turning on the spot (to make the game more dynamic)

    def initialise(self):
        super().initialise
        self.set_solidity(True)
        self.set_sprite("main/north")
        self.set_visible(True)
        self.__character_name = self.get_name()

    def get_character_name(self):
        return self.__character_name

    def set_character_name(self, character_name):
        self.__character_name = character_name

    def set_busy(self, busy, callback = lambda: None):
        self.__busy = busy
        callback()

    def is_busy(self):
        return self.__busy

    """ Change the sprite folder to "north" """
    def face_north(self, callback = lambda: None):
        self.__face("north", callback)
        return

    """ Change the sprite folder to "east" """
    def face_east(self, callback = lambda: None):
        self.__face("east", callback)
        return

    """ Change the sprite folder to "south" """
    def face_south(self, callback = lambda: None):
        self.__face("south", callback)
        return

    """ Change the sprite folder to "west" """
    def face_west(self, callback = lambda: None):
        self.__face("west", callback)
        return

    """ Change the sprite to face towards the given object """
    def turn_to_face(self, object_towards, callback = lambda: None):
        xP, yP = object_towards.get_position()
        xS, yS = self.get_position()
        xDiff = xP - xS
        yDiff  = yP - yS

        if xDiff > 0:
            self.face_east(callback)
        elif xDiff < 0:
            self.face_west(callback)
        elif yDiff > 0:
            self.face_north(callback)
        elif yDiff < 0:
            self.face_south(callback)
        else:
            callback()
        return


    """ Get the character to "face" the direction specified
    simply changes the last part of the sprite folder as relevant
    """
    def __face(self, direction, callback):
        engine = self.get_engine()
        sprite_location = self.get_sprite()
        sprite_location = sprite_location[0 : sprite_location.rfind("/") + 1] #grab all the characters before the last "/" from the string (as sprite_location will be something like main/north)
        self.set_sprite(sprite_location + direction) #sprites are now looked for in direction folder :)
        engine.add_event(callback)
        return

    """ Says if the character is facing north """
    def is_facing_north(self):
        return self.__is_facing("north")

    """ Says if the character is facing east """
    def is_facing_east(self):
        return self.__is_facing("east")

    """ Says if the character is facing south """
    def is_facing_south(self):
        return self.__is_facing("south")

    """ Says if the character is facing west """
    def is_facing_west(self):
        return self.__is_facing("west")

    """ Checks to see if the character is facing in the direction given
    direction -- a string specifing the direction, either 'north', 'east', 'south' or 'west'
    """
    def __is_facing(self, direction):
        sprite_location = self.get_sprite()  # The direction is related to where the sprite is stored, so extract the information from there
        sprite_location = sprite_location[sprite_location.rfind("/") + 1 : ] # Slice all the characters before the last "/" from the string
        return (sprite_location == direction)

    """ Moves the character in the given direction by one tile and animates them.

    They will only move and animate if they are not busy.
    face_x -- the function to make the character face in the correct direction
    parent_move_x -- the parent move function, calls the engine api but doesn't start the animation
    callback -- the function that you would like to call after the movement is complete
    """
    def __move_x(self, face_x, parent_move_x, callback):
        if not(self.is_busy()):
            face_x()
            self.start_animating()
            parent_move_x(lambda: self.__stop_animating_func(callback))
        return

    def __stop_animating_func(self, callback):
        self.get_engine().add_event(callback)
        self.wait(0.03, self.__check_stop_animating)

    def __check_stop_animating(self):
        if not self.is_moving():
            self.stop_animating()

    def move_on_spot(self, callback = lambda: None):
        self.start_animating()
        self.move_by((0, 0), time = 0.3, callback = lambda: self.__stop_animating_func(callback))

    """ Moves the character North by one tile and makes them face in that direction
    callback -- the function that you would like to call after the movement is complete
    """
    def move_north(self, callback = lambda: None):
        self.__move_x(self.face_north, super().move_north, callback)
        return

    """ Move character in direction by one tile.
    Overides general object implementation
    """
    def move_east(self, callback = lambda: None):
        self.__move_x(self.face_east, super().move_east, callback)
        return

    """ Move character in direction by one tile.
    Overides general object implementation
    """
    def move_south(self, callback = lambda: None):
        self.__move_x(self.face_south, super().move_south, callback)
        return

    """ Move character in direction by one tile.
    Overides general object implementation
    """
    def move_west(self, callback = lambda: None):
        self.__move_x(self.face_west, super().move_west, callback)
        return

    def change_state(self, state):
        sprite_location = self.get_sprite()
        sprite_location = sprite_location[sprite_location.rfind("/"): ]
        self.set_sprite(state + sprite_location)
        return


    def follow(self, game_object, callback = lambda: None):
        self.__follow_loop(game_object)
        self.get_engine().add_event(callback)

    def __follow_loop(self, game_object):
        """ Basic AI for the argument character to follow the player """
        engine = self.get_engine()
        xP, yP = game_object.get_position()
        xC, yC = self.get_position()
        xD, yD = (xP - xC, yP - yC)
        if(abs(xD) > abs(yD)):
            if(xD > 1):
                if not engine.is_solid((xC + 1, yC)):
                    self.move_east(callback = lambda: self.__follow_loop(game_object))
                else:
                    if(yD > 0):
                        self.move_north(callback = lambda: self.__follow_loop(game_object))
                    elif(yD < 0):
                        self.move_south(callback = lambda: self.__follow_loop(game_object))
                    else:
                        self.wait(0.3, callback = lambda: self.__follow_loop(game_object))
            elif(xD == 1):
                self.face_east(callback = lambda: self.wait(0.3, callback = lambda: self.__follow_loop(game_object)))
            elif(xD < -1):
                if not engine.is_solid((xC - 1, yC)):
                    self.move_west(callback = lambda: self.__follow_loop(game_object))
                else:
                    if(yD > 0):
                        self.move_north(callback = lambda: self.__follow_loop(game_object))
                    elif(yD < 0):
                        self.move_south(callback = lambda: self.__follow_loop(game_object))
                    else:
                        self.wait(0.3, callback = lambda: self.__follow_loop(game_object))
            elif(xD == -1):
                self.face_west(callback = lambda: self.wait(0.3, callback = lambda: self.__follow_loop(game_object)))
            else:
                self.wait(0.3, callback = lambda: self.__follow_loop(game_object))
        else:
            if(yD > 1):
                if not engine.is_solid((xC, yC + 1)):
                    self.move_north(callback = lambda: self.__follow_loop(game_object))
                else:
                    if(xD > 0):
                        self.move_east(callback = lambda: self.__follow_loop(game_object))
                    elif(xD < 0):
                        self.move_west(callback = lambda: self.__follow_loop(game_object))
                    else:
                        self.wait(0.3, callback = lambda: self.__follow_loop(game_object))

            elif(yD == 1):
                self.face_north(callback = lambda: self.wait(0.3, callback = lambda: self.__follow_loop(game_object)))
            elif(yD < -1):
                if not engine.is_solid((xC, yC - 1)):
                    self.move_south(callback = lambda: self.__follow_loop(game_object))
                else:
                    if(xD > 0):
                        self.move_east(callback = lambda: self.__follow_loop(game_object))
                    elif(xD < 0):
                        self.move_west(callback = lambda: self.__follow_loop(game_object))
                    else:
                        self.wait(0.3, callback = lambda: self.__follow_loop(game_object))
            elif(yD == -1):
                self.face_south(callback = lambda: self.wait(0.3, callback = lambda: self.__follow_loop(game_object)))
            else:
                self.wait(0.3, callback = lambda: self.__follow_loop(game_object))

    #Start the character turning randomly on the spot.
    #If frequency is 3, then the player with repeatedly face in different directions.
    #The greater frequency is, the less often the player will turn
    def start_turning(self, time = 0.5, frequency = 8, callback = lambda: None):
        if frequency < 3:
            self.__turning(time, 3, callback = callback)
        else:
            self.__turning(time, frequency, callback = callback)

    #Stop the character turning on the spot
    def stop_turning(self, callback = lambda: None):
        self.__finish_turning = True
        callback()

    #Recusively turn the character around
    def __turning(self, time = 0.5, frequency = 8, callback = lambda: None):
        direction = randint(0,frequency)
        if (self.__finish_turning):
            finish_turning = False
            callback()
            return
        elif direction == 0:
            self.face_north(lambda: self.wait(time, callback = lambda: self.__turning(time, frequency)))
        elif direction == 1:
            self.face_east(lambda: self.wait(time, callback = lambda: self.__turning(time, frequency)))
        elif direction == 2:
            self.face_south(lambda: self.wait(time, callback = lambda: self.__turning(time, frequency)))
        elif direction == 3:
            self.face_west(lambda: self.wait(time, callback = lambda: self.__turning(time, frequency)))
        else:
            self.wait(time, callback = lambda: self.__turning(time, frequency))
        callback()


#Python modules
import operator
import os
import sys
from random import randint

"""
In Python comments,
could define some standard which the C++ code can use to determine things about it handles
the python code
"""

#Custom modules
sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/../../script_running')
import scriptrunner
from script_state_container import ScriptStateContainer

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

class Character(GameObject, ScriptStateContainer):

    __character_name = ""

    __busy = False #A business flag used in some circumstances to check if the character is already occupied with something else to make sure certain things don't clash

    __finish_turning = False#A flag to specify whether the character is finsihing turning (to make the game more dynamic)

    __finish_following = False#A flag to specify whether the character is finishing another object(to make the game more dynamic)

    __cuts_left = 0

    __keys = 0

    def initialise(self):
        super().initialise
        self.set_solidity(True)
        self.set_sprite("main/north")
        self.set_visible(True)
        self.__character_name = self.get_name()


    """ ---- All code to do with running player scripts (also see inherited ScriptStateContainer) ---- """

    def run_script(self, script_to_run = -1, parse_error = False, callback = lambda: None):
        """ Runs the current script in the player_scripts folder in a seperate thread. Exposes the PyGuide API to the script to allow it to control this player. :)

        Everything in the API is bocking, however this doesn't impact regular gameplay as it's run in a seperate thread.
        The callback is run after the script has finished running.
        """

        engine = self.get_engine()
        if not(self.is_running_script()): #only run script if one currently isn't running.
            if (script_to_run == -1):
                script_to_run = engine.get_run_script()

            self.set_running_script_status(True)

            #script_api is a python dictionary of python objects (variables, methods, class instances etc.)
            #available to the player. :)
            #eg. if there is an entry named "fart" whos entry is blob, then in the level script, any reference to fart
            #will be refering to what blob is known as here.
            #Here the list of game_objects is being looped through, and their names are being mapped to each instance :)
            script_api = {}

            # Provide all the movement functions to the player, but make them blocking.
            script_api["move_north"] = scriptrunner.make_blocking(self.move_north)
            script_api["move_east"] = scriptrunner.make_blocking(self.move_east)
            script_api["move_south"] = scriptrunner.make_blocking(self.move_south)
            script_api["move_west"] = scriptrunner.make_blocking(self.move_west)


            script_api["face_north"] = scriptrunner.make_blocking(self.face_north)
            script_api["face_east"] = scriptrunner.make_blocking(self.face_east)
            script_api["face_south"] = scriptrunner.make_blocking(self.face_south)
            script_api["face_west"] = scriptrunner.make_blocking(self.face_west)
            script_api["turn_left"] = scriptrunner.make_blocking(self.__turn_left)
            script_api["turn_right"] = scriptrunner.make_blocking(self.__turn_right)

            script_api["can_move"] = self.__can_move
            script_api["move"] = scriptrunner.make_blocking(self.__move)

            script_api["wait"] = scriptrunner.make_blocking(lambda callback: self.wait(0.3, callback))

            #the method to get the position of the player
            script_api["get_position"] = self.get_position
            script_api["get_flag_message"] = self.get_flag_message

            script_api["yell"] = self.__yell
            script_api["cut"] = scriptrunner.make_blocking(self.__cut)

            scriptrunner.start(script_api, script_to_run, self, engine, parse_error, callback)
        return

    #override ScriptStateContainer
    def get_script_name(self):
        return self.get_character_name()

    #override ScriptStateContainer---
    def set_script_name(self):
        self.set_character_name(self)

    def get_character_name(self):
        return self.__character_name.capitalize()

    def set_character_name(self, character_name):
        self.__character_name = character_name

    def set_busy(self, busy, callback = lambda: None):
        self.__busy = busy
        self.halt_script() #Halt the player script
        self.get_engine().add_event(callback)

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

    def __turn_left(self, callback = lambda: None):
        if self.is_facing_east():
            self.face_north(callback)
        elif self.is_facing_north():
            self.face_west(callback)
        elif self.is_facing_west():
            self.face_south(callback)
        elif self.is_facing_south():
            self.face_east(callback)

    def __turn_right(self, callback = lambda: None):
        if self.is_facing_east():
            self.face_south(callback)
        elif self.is_facing_north():
            self.face_east(callback)
        elif self.is_facing_west():
            self.face_north(callback)
        elif self.is_facing_south():
            self.face_west(callback)

    def __can_move(self):
        engine = self.get_engine()
        x,y = self.get_position()
        try:
            if self.is_facing_east():
                return not engine.is_solid((x+1,y))
            elif self.is_facing_north():
                return not engine.is_solid((x,y+1))
            elif self.is_facing_west():
                return not engine.is_solid((x-1,y))
            elif self.is_facing_south():
                return not engine.is_solid((x,y-1))
        except IndexError:
            return False


    def __move(self, callback = lambda: None):
        if self.is_facing_east():
            self.move_east(callback)
        elif self.is_facing_north():
            self.move_north(callback)
        elif self.is_facing_west():
            self.move_west(callback)
        elif self.is_facing_south():
            self.move_south(callback)
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

    #Stop following whichever object the character was set to follow
    def stop_follow(self, callback = lambda: None):
        self.__finish_following = True
        self.get_engine().add_event(callback)

    def __follow_loop(self, game_object):
        """ Basic AI for the argument character to follow the player, isn't solid when still so that player doesn't get stuck TODO: find a better compromise!!! """
        if(self.__finish_following):
            self.__finish_following = False
            return
        engine = self.get_engine()
        self.set_solidity(False)
        xP, yP = game_object.get_position()
        xC, yC = self.get_position()
        xD, yD = (xP - xC, yP - yC)
        if(abs(xD) > abs(yD)):
            if(xD > 1):
                if not engine.is_solid((xC + 1, yC)):
                    self.set_solidity(True)
                    self.move_east(callback = lambda: self.__follow_loop(game_object))
                else:
                    if(yD > 0):
                        self.set_solidity(True)
                        self.move_north(callback = lambda: self.__follow_loop(game_object))
                    elif(yD < 0):
                        self.set_solidity(True)
                        self.move_south(callback = lambda: self.__follow_loop(game_object))
                    else:
                        self.wait(0.3, callback = lambda: self.__follow_loop(game_object))
            elif(xD == 1):
                self.face_east(callback = lambda: self.wait(0.3, callback = lambda: self.__follow_loop(game_object)))
            elif(xD < -1):
                if not engine.is_solid((xC - 1, yC)):
                    self.set_solidity(True)
                    self.move_west(callback = lambda: self.__follow_loop(game_object))
                else:
                    if(yD > 0):
                        self.set_solidity(True)
                        self.move_north(callback = lambda: self.__follow_loop(game_object))
                    elif(yD < 0):
                        self.set_solidity(True)
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
                    self.set_solidity(True)
                    self.move_north(callback = lambda: self.__follow_loop(game_object))
                else:
                    if(xD > 0):
                        self.set_solidity(True)
                        self.move_east(callback = lambda: self.__follow_loop(game_object))
                    elif(xD < 0):
                        self.set_solidity(True)
                        self.move_west(callback = lambda: self.__follow_loop(game_object))
                    else:
                        self.wait(0.3, callback = lambda: self.__follow_loop(game_object))

            elif(yD == 1):
                self.face_north(callback = lambda: self.wait(0.3, callback = lambda: self.__follow_loop(game_object)))
            elif(yD < -1):
                if not engine.is_solid((xC, yC - 1)):
                    self.set_solidity(True)
                    self.move_south(callback = lambda: self.__follow_loop(game_object))
                else:
                    if(xD > 0):
                        self.set_solidity(True)
                        self.move_east(callback = lambda: self.__follow_loop(game_object))
                    elif(xD < 0):
                        self.set_solidity(True)
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
        self.get_engine().add_event(callback)

    #Recusively turn the character around
    def __turning(self, time = 0.5, frequency = 8, callback = lambda: None):
        direction = randint(0,frequency)
        if (self.__finish_turning):
            self.__finish_turning = False
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
        self.get_engine().add_event(callback)

    def get_flag_message(self):
        message = "There is no flag here!"
        engine = self.get_engine()
        position = self.get_position()
        game_objects = engine.get_objects_at(position)
        for current_object in game_objects:
            if(hasattr(current_object, "get_message")):
                message = current_object.get_message()
        return message

    def __yell(self):
        engine = self.get_engine()
        objects = engine.get_all_objects()
        for current in objects:
            if hasattr(current, "yelled_at"):
                current.yelled_at(self)

    def set_cuts_left(self, cuts_left):
        self.__cuts_left = cuts_left

    def get_cuts_left(self):
        return self.__cuts_left

    def add_keys(self, num):
        self.__keys += num

    def get_keys(self):
        return self.__keys

    def give_keys(self):
        pass

    def __cut(self, callback = lambda: None):
        engine = self.get_engine()

        if (self.__cuts_left == 0):
            engine.add_event(callback)
            engine.print_terminal("Not enough cuts left!")
            return False

        (x,y) = self.get_position()

        made_cut = False

        if self.is_facing_east():
            for obj in engine.get_objects_at((x+1, y)):
                if(hasattr(obj, "on_cut")):
                    made_cut = obj.on_cut(callback = callback)
                    break

        elif self.is_facing_west():
            for obj in engine.get_objects_at((x-1, y)):
                if(hasattr(obj, "on_cut")):
                    made_cut = obj.on_cut(callback = callback)
                    break

        elif self.is_facing_north():
            for obj in engine.get_objects_at((x, y+1)):
                if(hasattr(obj, "on_cut")):
                    made_cut = obj.on_cut(callback = callback)
                    break

        elif self.is_facing_south():
            for obj in engine.get_objects_at((x, y-1)):
                if(hasattr(obj, "on_cut")):
                    made_cut = obj.on_cut(callback = callback)
                    break

        if made_cut:
            self.__cuts_left = self.__cuts_left - 1
            if (self.__cuts_left == 0):
                engine.print_terminal("Swoosh! Ran out of cuts")
            else:
                engine.print_terminal("Swoosh! This knife now has " + str(self.__cuts_left) + " cut(s) left!")
        else:
            engine.print_terminal("Swish? There's nohing to cut. This knife still has " + str(self.__cuts_left) + " cut(s) left!")
            engine.add_event(callback)
        return made_cut







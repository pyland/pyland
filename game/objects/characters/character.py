#Python modules
import operator
import os
import sys
from random import randint

#Custom modules
sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/../../script_running')
import scriptrunner
from script_state_container import ScriptStateContainer
sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/..')
from game_object import GameObject


class Character(GameObject, ScriptStateContainer):
    """ This class is the parent of all characters in the game.

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

    __character_name = ""

    __busy = False #A business flag used in some circumstances to check if the character is already occupied with something else to make sure certain things don't clash

    __finish_turning = False #A flag to specify whether the character is finsihing turning (to make the game more dynamic)

    __finish_following = False #A flag to specify whether the character is finishing another object(to make the game more dynamic)

    __cuts_left = 0

    __keys = 0

    __script = "" #The script that the NPC stores (for when the player is writing scripts for them)

    def initialise(self):
        super().initialise
        self.set_solidity(True)
        self.set_sprite("main/north")
        self.set_visible(True)
        self.__character_name = self.get_name()


    """ ---- All code to do with running player scripts (also see inherited ScriptStateContainer) ---- """

    def run_script(self, script_to_run = -1, parse_error = True, callback = lambda: None):
        """ Runs the current script in the player_scripts folder in a seperate thread. Exposes the PyGuide API to the script to allow it to control this player.

        Everything in the API is bocking, however this doesn't impact regular gameplay as it's run in a seperate thread.


        Parameters
        ----------
        script_to_run : int, optional
            A flag that indicates which script to run. -1 indicates the current script
        parse_error : bool, optional
            #TODOFILL THIS
        callback : func, optional
            Places the callback onto the engine event-queue
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

            script_api["scan"] = self.__scan

            script_api["wait"] = scriptrunner.make_blocking(lambda callback: self.wait(0.3, callback))

            #the method to get the position of the player
            script_api["get_position"] = self.get_position

            script_api["yell"] = self.__yell
            script_api["cut"] = scriptrunner.make_blocking(self.__cut)
            script_api["get_cuts_left"] = self.__get_cuts_left

            script_api["dig"] = self.dig


            scriptrunner.start(script_api, script_to_run, self, engine, parse_error, callback)
        return

    #override ScriptStateContainer
    def get_script_name(self):
        """ This is used by the runscript to identify each character thread as unique
        Returns
        -------
        str
            String of the character name
        """
        return self.get_character_name()

    #override ScriptStateContainer---
    def set_script_name(self):
        """ Sets the character name to the name of the object
        """
        self.set_character_name(self)

    def set_script(self, script):
        """ Sets the character's script to be the text 'script'
        """
        self.__script = script

    def get_script(self):
        """
        Returns
        -------
        str
            A string of the player's script text
        """
        return self.__script

    def get_character_name(self):
        """
        Returns
        -------
        str
            A capitalized string of the character's name
        """
        return self.__character_name.capitalize()

    def set_character_name(self, character_name):
        """ Sets the character name to the inputted character_name string

        Parameters
        ----------
        character_name : str
            A string indicating the new name of the character
        """
        self.__character_name = character_name

    def set_busy(self, busy, callback = lambda: None):
        """ Sets the play to busy or un-busy.

        If the player is busy then he cannot move or run scripts.

        Parameters
        ----------
        busy : bool
            A boolean value indicating if the player should be set to busy
        callback : func, optional
            Places the callback onto the engine event-queue
        """
        self.__busy = busy
        self.halt_script() #Halt the player script
        self.get_engine().add_event(callback)

    def is_busy(self):
        """
        Returns
        -------
        bool
            A boolean value indicating if the character is busy or not
        """
        return self.__busy

    def face_north(self, callback = lambda: None):
        """ Change the sprite folder to "north"

        Parameters
        ----------
        callback : func, optional
            Places the callback onto the engine event-queue
        """

        self.__face("north", callback)
        return

    def face_east(self, callback = lambda: None):
        """ Change the sprite folder to "east"

        Parameters
        ----------
        callback : func, optional
            Places the callback onto the engine event-queue
        """
        self.__face("east", callback)
        return

    def face_south(self, callback = lambda: None):
        """ Change the sprite folder to "south"

        Parameters
        ----------
        callback : func, optional
            Places the callback onto the engine event-queue
        """
        self.__face("south", callback)
        return

    def face_west(self, callback = lambda: None):
        """ Change the sprite folder to "west"

        Parameters
        ----------
        callback : func, optional
            Places the callback onto the engine event-queue
        """

        self.__face("west", callback)
        return

    def turn_to_face(self, object_towards, callback = lambda: None):
        """ Change the sprite to face towards the given object"

        Parameters
        ----------
        object_towards : GameObject
            The GameObject that we want the current object to face towards.
        callback : func, optional
            Places the callback onto the engine event-queue
        """
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


    def toggle(self, callback = lambda: None):
        """ Toggles the Character and makes him face the opposite direction
        """
        if(self.is_facing_north()):
            return self.face_south(callback)
        if(self.is_facing_south()):
            return self.face_north(callback)
        if(self.is_facing_east()):
            return self.face_west(callback)
        if(self.is_facing_west()):
            return self.face_east(callback)


    def __turn_left(self, callback = lambda: None):
        """ Makes the character turn left

        Parameters
        ----------
        callback : func, optional
            Places the callback onto the engine event-queue
        """

        if self.is_facing_east():
            self.face_north(callback)
        elif self.is_facing_north():
            self.face_west(callback)
        elif self.is_facing_west():
            self.face_south(callback)
        elif self.is_facing_south():
            self.face_east(callback)

    def __turn_right(self, callback = lambda: None):
        """ Makes the character turn right

        Parameters
        ----------
        callback : func, optional
            Places the callback onto the engine event-queue
        """
        if self.is_facing_east():
            self.face_south(callback)
        elif self.is_facing_north():
            self.face_east(callback)
        elif self.is_facing_west():
            self.face_north(callback)
        elif self.is_facing_south():
            self.face_west(callback)

    def __can_move(self):
        """
        Returns
        -------
        bool
            A boolean value indicating if the tile in front of the character can be moved to
        """
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
        """ If possible, moves the character forward one space

        Parameters
        ----------
        callback : func, optional
            Places the callback onto the engine event-queue
        """

        if self.is_facing_east():
            self.move_east(callback)
        elif self.is_facing_north():
            self.move_north(callback)
        elif self.is_facing_west():
            self.move_west(callback)
        elif self.is_facing_south():
            self.move_south(callback)

    def __face(self, direction, callback):

        """ Get the character to "face" the direction specified

        This method simply changes the last part of the sprite folder as relevant

        Parameters
        ----------
        direction : str
            A direction string. This should be in ["north", "east", "south", "west"]
        callback : func, optional
            Places the callback onto the engine event-queue

        """
        engine = self.get_engine()
        sprite_location = self.get_sprite()
        sprite_location = sprite_location[0 : sprite_location.rfind("/") + 1] #grab all the characters before the last "/" from the string (as sprite_location will be something like main/north)
        self.set_sprite(sprite_location + direction) #sprites are now looked for in direction folder :)
        engine.add_event(callback)
        return

    def is_facing_north(self):
        """
        Returns
        -------
        bool
            A boolean value indicating if the player is facing north.
        """

        return self.__is_facing("north")

    def is_facing_east(self):
        """
        Returns
        -------
        bool
            A boolean value indicating if the player is facing east.
        """
        return self.__is_facing("east")

    def is_facing_south(self):
        """
        Returns
        -------
        bool
            A boolean value indicating if the player is facing south.
        """
        return self.__is_facing("south")

    def is_facing_west(self):
        """
        Returns
        -------
        bool
            A boolean value indicating if the player is facing west.
        """
        return self.__is_facing("west")


    def __is_facing(self, direction):
        """ Checks to see if the character is facing in the direction given
        direction -- a string specifing the direction, either 'north', 'east', 'south' or 'west'

        Parameters
        ----------
        direction : str
            A directional string ("north", "east", "south", "west") that we want to check if the character is facing

        Returns
        -------
        bool
            A boolean value indicating if the player is facing in the direction specified.
        """
        sprite_location = self.get_sprite()  # The direction is related to where the sprite is stored, so extract the information from there
        sprite_location = sprite_location[sprite_location.rfind("/") + 1 : ] # Slice all the characters before the last "/" from the string
        return (sprite_location == direction)

    def __move_x(self, face_x, parent_move_x, callback = lambda: None):
        """ Moves the character in the given direction by one tile and animates them.

        They will only move and animate if they are not busy.

        Parameters
        ----------
        face_x : func
            The function to make the character face in the correct direction
        parent_move_x : func
            The parent move function, calls the engine api but doesn't start the animation
        callback : func, optional
            Places the callback onto the engine event-queue
        """
        face_x()
        self.start_animating()
        parent_move_x(lambda: self.__stop_animating_func(callback))
        return

    def __stop_animating_func(self, callback = lambda : None):
        """ After 0.03 the character stops animating.

        Parameters
        ----------
        callback : func, optional
            Places the callback onto the engine event-queue
        """
        self.wait(0.03, self.__check_stop_animating)
        self.get_engine().add_event(callback)


    def __check_stop_animating(self):
        """
        Returns
        -------
        bool
            A boolean value indicating if the character is being animated or not
        """
        if not self.is_moving():
            self.stop_animating()

    def move_on_spot(self, callback = lambda: None):
        """ Makes the character walk in place. Runs the animation but the position of the character does not changes

        Parameters
        ----------
        callback : func, optional
            Places the callback onto the engine event-queue

        """
        self.start_animating()
        self.move_by((0, 0), time = 0.3, callback = lambda: self.__stop_animating_func(callback))


    def move_north(self, callback = lambda: None):
        """ Moves the character north by one tile and makes them face in that direction

        Parameters
        ----------
        callback : func, optional
            Places the callback onto the engine event-queue
        """
        self.__move_x(self.face_north, super().move_north, callback)
        return

    def move_east(self, callback = lambda: None):
        """ Moves the character east by one tile and makes them face in that direction.

        Overides general object implementation

        Parameters
        ----------
        callback : func, optional
            Places the callback onto the engine event-queue
        """

        self.__move_x(self.face_east, super().move_east, callback)
        return

    def move_south(self, callback = lambda: None):
        """ Moves the character south by one tile and makes them face in that direction.

        Overides general object implementation

        Parameters
        ----------
        callback : func, optional
            Places the callback onto the engine event-queue
        """

        self.__move_x(self.face_south, super().move_south, callback)
        return

    def move_west(self, callback = lambda: None):
        """ Moves the character west by one tile and makes them face in that direction.

        Overides general object implementation
        Parameters
        ----------
        callback : func, optional
            Places the callback onto the engine event-queue
        """
        self.__move_x(self.face_west, super().move_west, callback)
        return

    def change_state(self, state):
        """ Changes the state of the character.

        A character can have multiple "states". e.g. a crocodile can have a main state and a swim state. If a crocodile is in the swim state, then the animation frames to be used should be the swimming frames.

        Parameters
        ----------
        state : str,
            A string indicating the state that the character should be changed to. e.g. "swim", "main".
        """

        sprite_location = self.get_sprite()
        sprite_location = sprite_location[sprite_location.rfind("/"): ]
        self.set_sprite(state + sprite_location)
        return


    def follow(self, game_object, callback = lambda: None):
        """ The current character follows the game_object specified.

        The follow algorithm is implemented in __follow_loop.

        Parameters
        ----------
        game_objects : GameObject
            The GameObject that the character will follow
        callback : func, optional
            Places the callback onto the engine event-queue
        """

        self.__follow_loop(game_object)
        self.get_engine().add_event(callback)


    def stop_follow(self, callback = lambda: None):
        """ The character will stop following all GameObject it has been set to follow

        callback : func, optional
            Places the callback onto the engine event-queue
        """

        self.__finish_following = True
        self.get_engine().add_event(callback)

    def __follow_loop(self, game_object):
        """ Basic AI for the argument character to follow a GameObject.

        The characters isn't solid when still so that player doesn't get stuck
        TODO: find a better compromise!

        Parameters
        ----------
        game_object : GameObject,
            The GameObject our character will follow.
        """

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
                if not ((engine.is_solid((xC + 1, yC)) or (engine.get_tile_type((xC + 1, yC)) == engine.TILE_TYPE_WATER))):
                    self.set_solidity(True)
                    self.move_east(callback = lambda: self.__follow_loop(game_object))
                else:
                    if(yD > 0 and not (engine.get_tile_type((xC, yC + 1)) == engine.TILE_TYPE_WATER)):
                        self.set_solidity(True)
                        self.move_north(callback = lambda: self.__follow_loop(game_object))
                    elif(yD < 0 and not (engine.get_tile_type((xC, yC - 1)) == engine.TILE_TYPE_WATER)):
                        self.set_solidity(True)
                        self.move_south(callback = lambda: self.__follow_loop(game_object))
                    else:
                        self.wait(0.3, callback = lambda: self.__follow_loop(game_object))
            elif(xD == 1):
                self.face_east(callback = lambda: self.wait(0.3, callback = lambda: self.__follow_loop(game_object)))
            elif(xD < -1):
                if not ((engine.is_solid((xC - 1, yC)) or (engine.get_tile_type((xC - 1, yC)) == engine.TILE_TYPE_WATER))):
                    self.set_solidity(True)
                    self.move_west(callback = lambda: self.__follow_loop(game_object))
                else:
                    if(yD > 0 and not (engine.get_tile_type((xC, yC + 1)) == engine.TILE_TYPE_WATER)):
                        self.set_solidity(True)
                        self.move_north(callback = lambda: self.__follow_loop(game_object))
                    elif(yD < 0 and not (engine.get_tile_type((xC, yC - 1)) == engine.TILE_TYPE_WATER)):
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
                if not ((engine.is_solid((xC, yC + 1)) or (engine.get_tile_type((xC, yC + 1)) == engine.TILE_TYPE_WATER))):
                    self.set_solidity(True)
                    self.move_north(callback = lambda: self.__follow_loop(game_object))
                else:
                    if(xD > 0 and not (engine.get_tile_type((xC + 1, yC)) == engine.TILE_TYPE_WATER)):
                        self.set_solidity(True)
                        self.move_east(callback = lambda: self.__follow_loop(game_object))
                    elif(xD < 0 and not (engine.get_tile_type((xC - 1, yC)) == engine.TILE_TYPE_WATER)):
                        self.set_solidity(True)
                        self.move_west(callback = lambda: self.__follow_loop(game_object))
                    else:
                        self.wait(0.3, callback = lambda: self.__follow_loop(game_object))

            elif(yD == 1):
                self.face_north(callback = lambda: self.wait(0.3, callback = lambda: self.__follow_loop(game_object)))
            elif(yD < -1):
                if not ((engine.is_solid((xC, yC - 1)) or (engine.get_tile_type((xC, yC - 1)) == engine.TILE_TYPE_WATER))):
                    self.set_solidity(True)
                    self.move_south(callback = lambda: self.__follow_loop(game_object))
                else:
                    if(xD > 0 and not (engine.get_tile_type((xC + 1, yC)) == engine.TILE_TYPE_WATER)):
                        self.set_solidity(True)
                        self.move_east(callback = lambda: self.__follow_loop(game_object))
                    elif(xD < 0 and not (engine.get_tile_type((xC - 1, yC)) == engine.TILE_TYPE_WATER)):
                        self.set_solidity(True)
                        self.move_west(callback = lambda: self.__follow_loop(game_object))
                    else:
                        self.wait(0.3, callback = lambda: self.__follow_loop(game_object))
            elif(yD == -1):
                self.face_south(callback = lambda: self.wait(0.3, callback = lambda: self.__follow_loop(game_object)))
            else:
                self.wait(0.3, callback = lambda: self.__follow_loop(game_object))


    def start_turning(self, time = 0.5, frequency = 8, callback = lambda: None): #Change frquency to period/sample
        """ Start the character turning randomly on the spot.

        The greater frequency is, the less often the player will turn
        """

        self.__finish_turning = False
        if frequency < 3:
            self.__turning(time, 3, callback = callback)
        else:
            self.__turning(time, frequency, callback = callback)

    def stop_turning(self, callback = lambda: None):
        """ Stops the character turning on the spot

        Parameters
        ----------
        callback : func, optional
            Places the callback onto the engine event-queue
        """

        self.__finish_turning = True
        self.get_engine().add_event(callback)

    def __turning(self, time = 0.5, frequency = 8, callback = lambda: None): #Change frequencey to period/sample
        """
        This method recursively turns the character around

        Parameters
        ----------
        time : double, optional
            The time in seconds in between turns
        frequency : int, optional

        callback : func, optional
            Places the callback onto the engine event-queue

        """
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

    def __scan(self):
        """
        Returns
        -------
        str
            String of the scroll in front of the player
        """
        message = "There is no message here!"
        engine = self.get_engine()
        x, y = self.get_position()

        if self.is_facing_east():
            for obj in engine.get_objects_at((x+1, y)):
                if(hasattr(obj, "on_scan")):
                    message = obj.on_scan()
                    break
        elif self.is_facing_west():
            for obj in engine.get_objects_at((x-1, y)):
                if(hasattr(obj, "on_scan")):
                    message = obj.on_scan()
                    break
        elif self.is_facing_north():
            for obj in engine.get_objects_at((x, y+1)):
                if(hasattr(obj, "on_scan")):
                    message = obj.on_scan()
                    break
        elif self.is_facing_south():
            for obj in engine.get_objects_at((x, y-1)):
                if(hasattr(obj, "on_scan")):
                    message = obj.on_scan()
                    break
        return message

    def __yell(self):
        """ Loops through all the GameObjects and if they have a yelled_at() function runs it.

        This is used for when you yell at crocodiles/anything else that can get yelled at
        """

        engine = self.get_engine()
        objects = engine.get_all_objects()
        for current in objects:
            if hasattr(current, "yelled_at"):
                current.yelled_at(self)

    def set_cuts_left(self, cuts_left):
        """ Sets the number of cuts the player has to cuts_left

        Parameters
        ----------
        cuts_left : int
            An integer indicating how many cuts the player has
        """
        self.__cuts_left = cuts_left

    def get_cuts_left(self):
        """
        Returns
        -------
        int
            Number of cuts the player has left
        """
        return self.__cuts_left

    def add_keys(self, num): #Move to level
        """ Increments the number of keys the player has by num.

        This is used the the "math" challenge where we have to collect two keys

        Parameters
        ----------
        num : int
            Integer we want to increment the number of keys the player has by
        """
        self.__keys += num

    def get_keys(self):
        """
        Returns
        -------
        int
            Number of keys the player current has
        """
        return self.__keys

    def __cut(self, callback = lambda: None):
        """ Tries to cut the object in front of the player. This is done by running the on_cut method if the object in front of the character has one

        First checks if the character has enough cuts left, if not it prints an error. Otherwise it runs the on_cut method

        Parameters
        ----------
        callback : func, optional
            Places the callback onto the engine event-queue

        """
        engine = self.get_engine()

        if (self.__cuts_left == 0):
            engine.add_event(callback)
            engine.print_terminal("The knife is blunt. No cuts left.")
            return False

        (x,y) = self.get_position()

        object_responded = False
        made_cut = False

        if self.is_facing_east():
            for obj in engine.get_objects_at((x+1, y)):
                if(hasattr(obj, "on_cut")):
                    made_cut = obj.on_cut(callback = callback)
                    object_responded = True
                    break
        elif self.is_facing_west():
            for obj in engine.get_objects_at((x-1, y)):
                if(hasattr(obj, "on_cut")):
                    made_cut = obj.on_cut(callback = callback)
                    object_responded = True
                    break
        elif self.is_facing_north():
            for obj in engine.get_objects_at((x, y+1)):
                if(hasattr(obj, "on_cut")):
                    made_cut = obj.on_cut(callback = callback)
                    object_responded = True
                    break
        elif self.is_facing_south():
            for obj in engine.get_objects_at((x, y-1)):
                if(hasattr(obj, "on_cut")):
                    made_cut = obj.on_cut(callback = callback)
                    object_responded = True
                    break

        if not object_responded:
            engine.add_event(callback)

        if made_cut:
            self.__cuts_left = self.__cuts_left - 1
            if self.__cuts_left  == 1:
                pass
                #engine.print_terminal("You just cut something, you have " + str(self.__cuts_left) + " cut left.")
            elif self.__cuts_left  == 0:
                pass
                #engine.print_terminal("You just cut something, your knife is now too blunt to cut anything else.")
            else:
                pass
                #engine.print_terminal("You just cut something, you have " + str(self.__cuts_left) + " cuts left.")
        return made_cut

    def __get_cuts_left(self):
        """
        Returns
        -------
        int
            Number of cuts the character has left
        """
        return self.__cuts_left

    def dig(self, callback = lambda: None):
        """ The player tries to dig at a location.

        This will usually be overridden in a level script if needed.

        Parameters
        ----------
        callback : func, optional
            Places the callback onto the engine event-queue
        """
        self.get_engine().print_terminal("You can't dig here!")
        self.get_engine().add_event(callback)


    def follow_path(self, path, repeat = False, is_paused = lambda: False):
        """ The character follows the path given, a string with comma-seperated directions, eg. "north, east, east, south, west, north"

        If repeat is set to True, the most recent direction completed will be added to the end of the string so that whole thing becomes a cycle

        Parameters
        ----------
        path : str
            A string with comma-seperated directions indicating the path of the object to follow
        repeat : bool, optional
            A boolean indicating if the path should be repeated after completion
        is_paused : void -> bool, optional
            A function which returns a boolean, checked on every call follow_path, if it returns True, the character will stop following the path until it returns False again

        """
        if(path.strip() == ""): #if path is empty terminate
            return

        if(is_paused()):
            self.wait(0.3, callback = lambda: self.follow_path(path, repeat = repeat, is_paused = is_paused))
            return

        engine = self.get_engine()
        x, y = self.get_position()

        comma_location = path.find(",") # Find the first comma in the path
        if(comma_location == -1):  # No commas in the path! On last word!
            comma_location = len(path)

        old_path = path #store the old_path

        instruction = path[ 0 : comma_location].strip() #get instruction and remove whitespace
        path = path[comma_location + 1: ].strip() #remove the instruction from the path itself
        if(repeat):
            path = path + ", " + instruction #add instruction back to the path, at the end if to be repeated
        if(instruction == "north"):
            if engine.is_solid((x, y+1)): #if position isn't walkable, then wait
                self.face_north()
                return self.wait(0.3, callback = lambda: self.follow_path(old_path, repeat = repeat, is_paused = is_paused))
            else:
                return self.move_north(callback = lambda: self.follow_path(path, repeat = repeat, is_paused = is_paused))
        elif(instruction == "east"):
            if engine.is_solid((x+1, y)): #if the position you are trying to move to is taken, wait
                self.face_east()
                return self.wait(0.3, callback = lambda: self.follow_path(old_path, repeat = repeat, is_paused = is_paused))
            else:
                return self.move_east(callback = lambda: self.follow_path(path, repeat = repeat, is_paused = is_paused))
            return
        elif(instruction == "south"):
            if engine.is_solid((x, y-1)):
                self.face_south()
                return self.wait(0.3, callback = lambda: self.follow_path(old_path, repeat = repeat, is_paused = is_paused))
            else:
                return self.move_south(callback = lambda: self.follow_path(path, repeat = repeat, is_paused = is_paused))
        elif(instruction == "west"):
            if engine.is_solid((x-1, y)):
                self.face_west()
                return self.wait(0.3, lambda: self.follow_path(old_path, repeat = repeat, is_paused = is_paused))
            else:
                return self.move_west(lambda: self.follow_path(path, repeat = repeat, is_paused = is_paused))
        elif(instruction == "face_north"):
            self.face_north(callback = lambda: self.follow_path(path, repeat = repeat, is_paused = is_paused))
        elif(instruction == "face_east"):
            self.face_east(callback = lambda: self.follow_path(path, repeat = repeat, is_paused = is_paused))
        elif(instruction == "face_south"):
            self.face_south(callback = lambda: self.follow_path(path, repeat = repeat, is_paused = is_paused))
        elif(instruction == "face_west"):
            self.face_west(callback = lambda: self.follow_path(path, repeat = repeat, is_paused = is_paused))
        elif(instruction == "pause"):
            self.wait(0.3, callback = lambda: self.follow_path(path, repeat = repeat, is_paused = is_paused))
        else:
            pass #TODO: handle invalid path!!!!!
            print(instruction)
            print(path)

        return




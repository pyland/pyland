import operator
import os
import sys

class GameObject:
    """ This is the base game object.

    Any object you wish to have in game MUST by a child of this.

    This acts as an abstraction of the C++ entity object and wraps around it.
    In cases where it is appropriate, it calls the entity methods directly, but other times
    makes their behaviour a bit nicer as well.
    """

    __name = ""
    __entity = None
    __engine = None

    def __init__(self):
        pass

    def initialise(self):
        """
            Yay!!!!
        """
        pass

    def set_entity(self, entity, engine):
        """ Set's the entity in the object.

        The entity is the instance of the C++ class that represent the entity in the game map
        this class essentially actus as a wrapper for it :) TODO: comment with references to bootstrapper
        Then performs all the neccesary initialisation :D

        Parameters
        ----------
        entity : Entity
            The C++ Entity instance that you wish to wrap.
        engine: Engine
            The C++ GameEngine instance, provides some usefule API.
        """
        self.__entity = entity
        self.set_sprite("")
        self.set_visible(False)
        self.set_solidity(False)
        self.__engine = engine

    def get_engine(self):
        return self.__engine

    def test(self, text):
        self.__entity.print_dialogue(text)
        return

    def callback_test(self, callback):
        self.__entity.callback_test(callback)
        return

    def wait(self, time, callback):
        self.__entity.wait(time, callback)
        return

    def get_name(self):
        return self.__entity.get_name()

    def set_sprite(self, sprite_location): #all sprites are relative to sprites/sprite_location/0.png , when objects are animated the engine automatically cycles through the numbered sprites in the folder
        self.__entity.set_sprite(sprite_location)
        return

    def get_sprite(self):
        return self.__entity.get_sprite()

    def is_solid(self):
        return self.__entity.is_solid()

    def set_solidity(self, solidity):
        self.__entity.set_solidity(solidity)
        return

    def is_visible(self):
        return True #TODO: implement this in entity

    def set_visible(self, visible):
        return

    def start_animating(self):
        #the api will start animating the sprite by cycling through the images in the given sprite_location folder!
        self.__entity.start_animating()
        return

    def stop_animating(self):
        """ Stops animating the object, reseting it back to the first frame. """
        self.pause_animating()
        self.__entity.set_animation_frame(0) #set animation back to first frame
        return

    def pause_animating(self):
        """ Pauses the animation of the object, displaying the current frame it's on. """
        self.__entity.pause_animating()
        return

    def get_number_of_animation_frames(self):
        return self.__entity.get_number_of_animation_frames()

    def focus(self):
        """ Centre the camera on the position of this object. And make the object the focus of input.

        If the object moves, then the camera follows it as well. This is the case until
        the focus of the camera gets changed to another object or the level is reloaded/changed,
        snaps instantly
        """
        self.__entity.focus()
        return

    def is_focus(self):
        """ Return if the object is being focused on by the camera
        """

        return self.__entity.is_focus()

    def get_position(self):
        """ Returns the location of the player as a 2-tuple.

        Returns
        -------
        tuple of int
            The first coordinate is their x-axis position,
            the second coordinate is their y-axis position,
        """
        return self.__entity.get_position() #TODO: implement stub

    def set_position(self, position):
        """ Sets the position of the object to be at the coordinates given.

        This happens instantly without any transition animations.

        Parameters
        ----------
        position : tuple of int
            The position you wish to move the object to.
        """
        #TODO: implement stub
        return

    def move_north(self, callback = lambda: None):
        """ Smoothly slides this object north by one tile.

        The callback is put on the event queue when the operation is complete.
        """
        self.__entity.move_north(callback)
        return

    def move_east(self, callback = lambda: None):
        """ Smoothly slides this object east by one tile.

        The callback is put on the event queue when the operation is complete.
        """
        self.__entity.move_east(callback)
        return

    def move_south(self, callback = lambda: None):
        """ Smoothly slides this object south by one tile.

        The callback is put on the event queue when the operation is complete.
        """
        self.__entity.move_south(callback)
        return

    def move_west(self, callback = lambda: None):
        """ Smoothly slides this object west by one tile.

        The callback is put on the event queue when the operation is complete
        """
        self.__entity.move_west(callback)
        return

    def moving(self):
        """ Returns if this object is moving.

        Returns
        -------
        bool
            True if the object is moving, false otherwise.
        """
        return False #TODO: implement stub

    def destroy(self, callback):
        """ Destroys the object (removes the instance from the map, and cleans up all information associated with it).
        callback gets run once the operation is complete.
        """
        #TODO: implement stub
        return

    def get_id(self):
        return self.__entity.get_id()



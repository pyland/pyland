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
    __visibility = False
    __entity = None  #An instance of Entity.hpp/cpp
    __engine = None  #An instanece of GameEngine.hpp/cpp

    def __init__(self):
        pass

    def initialise(self):
        """ This method can be overidden by child classes, it is used intead of __init__ so that instances of the classes can be guarenteed to have __entity and __engine set """
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
        self.set_visible(False)
        self.set_solidity(False)
        self.__engine = engine

    def get_engine(self):
        return self.__engine

    def callback_test(self, callback):
        """ This method can be used to test a callback, all it does is take a callback and put it onto the event queue to be run by the game the next frame """
        self.__entity.callback_test(callback)
        return

    def wait(self, time, callback):
        """ Calls the callback after a certain amount of time """
        self.__entity.wait(time, callback)
        return

    def get_name(self):
        return self.__entity.get_name()

    def get_file_location(self):
        return self.__entity.get_location()

    def set_sprite(self, sprite_location, callback = lambda : None): #all sprites are relative to sprites/sprite_location/0.png , when objects are animated the engine automatically cycles through the numbered sprites in the folder
        self.__entity.set_sprite(sprite_location, callback)
        return

    def get_sprite(self):
        return self.__entity.get_sprite()

    def is_solid(self):
        return self.__entity.is_solid()

    def set_solidity(self, solidity, callback = lambda : None):
        self.__entity.set_solidity(solidity, callback)
        return

    def is_visible(self):
        return self.__visibility

    def set_visible(self, visibility, callback = lambda : None):
        self.__visibility = visibility
        self.__entity.set_visible(visibility, callback)
        return

    #TODO: Make it so that the callbacks of the following methods get passed into c++ so they are put on the event queue as opposed to being run from Python!
    def start_animating(self, speed = 0.06, loop = True, callback = lambda: None):
        #the api will start animating the sprite by cycling through the images in the given sprite_location folder!
        self.__entity.start_animating(speed, loop)
        callback()
        return

    def stop_animating(self, callback = lambda: None):
        """ Stops animating the object, reseting it back to the first frame. """
        self.pause_animating()
        self.__entity.set_animation_frame(0) #set animation back to first frame
        callback()
        return

    def pause_animating(self, callback = lambda: None):
        """ Pauses the animation of the object, displaying the current frame it's on. """
        self.__entity.pause_animating()
        callback()
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
        position : 2 tuple of int
            The position you wish to move the object to.
        """
        #TODO: implement stub
        return

    def move_by(self, move_amount, time = 0.0, callback = lambda: None):
        """ Moves the object by the given amount in the game

        Parameters
        ----------
        move_amount : 2 tuple of int
            The amount you wish to move the object by
        time : double
            The amount of time the movent should take
        callback : function
            The callback that you wish to run once the movement is complete
        """
        x, y = move_amount
        self.__entity.move_by(x, y, time, callback)

    def move_to(self, target_position, time = 0.0, callback = lambda: None):
        """ Moves the object to the given target position in the game

        Parameters
        ----------
        target_position : 2 tuple of int
            The position you wish to move the object to
        time : double
            The amount of time the movent should take
        callback : function
            The callback that you wish to run once the movement is complete

        """
        xt, yt = target_position
        xp, yp = self.get_position()
        self.__entity.move_by(xt - xp, yt - yp, time, callback)


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

    def is_moving(self):
        """ Returns if this object is moving.

        Returns
        -------
        bool
            True if the object is moving, false otherwise.
        """
        return self.__entity.is_moving()

    def destroy(self, callback):
        """ Destroys the object (removes the instance from the map, and cleans up all information associated with it).
        callback gets run once the operation is complete.
        """
        #TODO: implement stub
        return

    def get_id(self):
        """ Return the entity id of the object, used for certain back-end features (such as retrieving objects at any given position """
        return self.__entity.get_id()



import operator
import os

import sys
sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/..')
from api_placeholder import API

api = API()

""" This is the base game object,
Any object you wish to have in game MUST by a child of this.

This acts as an abstraction of the C++ entity object and wraps around it.
In cases where it is appropriate, it calls the entity methods directly, but other times
makes their behaviour a bit nicer as well.
"""
class GameObject:

    __name = ""
    __entity = None

    def __init__(self):
        pass
    
    """ Set's the entity in the object. The entity is the instance of the C++ class that represent the entity in the game map
    this class essentially actus as a wrapper for it :) TODO: comment with references to bootstrapper
    Then performs all the neccesary initialisation :D
    """
    def set_entity(self, entity):
        self.__entity = entity
        self.set_sprite("")
        self.set_visible(False)
        self.set_solid(False)

    def test(self, text):
        self.__entity.print_dialogue(text)
        return

    def callback_test(self, callback):
        self.__entity.callback_test(callback)
        return

    def get_name(self):
        return self.__entity.get_name()

    def set_sprite(self, sprite_location): #all sprites are relative to sprites/sprite_location/0.png , when objects are animated the engine automatically cycles through the numbered sprites in the folder
        api.set_sprite(self, sprite_location)
        
    def get_sprite(self):
        return api.get_sprite(self)

    def is_solid(self):
        return api.is_solid(self)

    def set_solid(self, solid):
        api.set_solid(self, solid)
        return

    def is_visible(self):
        return api.is_visible(self)

    def set_visible(self, visible):
        api.set_visible(self, visible)
        return

    def start_animating(self):
        api.start_animating(self) #the api will start animating the sprite by cycling through the images in the given sprite_location folder!
        return

    def stop_animating(self):
        self.pause_animating()
        api.set_frame(self, 0) #set animation back to first frame
        return

    def pause_animating(self):
        api.pause_animating(self) #api will pause the animation
        return

    """ Centre the camera on the position of this object, if the object moves, then
    the camera follows it as well. This is the case until the focus of the camera
    gets changed to another object or the level is reloaded/changed, snaps instantly """
    def focus(self):
        self.__entity.focus()
        return

    """ Returns the location of the player,
    the first coordinate is their x-axis position
    the second coordinate is their y-axis position
    the third coordinate is the layer they are in. ??
    """
    def get_position(self):
        return api.get_position(self) #ask api to return an instance of self

    """ Sets the position of the object to be at the
    coordinates given. This method works without any
    transition animations.
    """
    def set_position(self, position):
        api.set_position(self, position)
        return
    
    """ Smoothly slides this object north by one tile 
    The callback is called when the operation is complete
    """
    def move_north(self, callback = lambda: None):
        #api.move_north(self, callback)
        self.__entity.move_north(callback)
        return

    """ Smoothly slides this object east by one tile 
    The callback is called when the operation is complete
    """
    def move_east(self, callback = lambda: None):
        #api.move_east(self, callback)
        self.__entity.move_east(callback)
        return

    """ Smoothly slides this object south by one tile 
    The callback is called when the operation is complete
    """
    def move_south(self, callback = lambda: None):
        #api.move_south(self, callback)
        self.__entity.move_south(callback)
        return

    """ Smoothly slides this object west by one tile 
    The callback is called when the operation is complete
    """
    def move_west(self, callback = lambda: None):
        #api.move_west(self, callback)
        self.__entity.move_west(callback)
        return

    """ Returns if this object is moving """
    def moving(self):
        return api.moving(self)
    
    """ Returns destroys the object (removes the instance from the map, and cleans up all information associated with it)
    callback gets run once the operation is complete
    """
    def destroy(self, callback):
        api.destroy(self, callback)



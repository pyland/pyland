import operator
import os

class GameObject: #An example of the base game object
    __visible = False;
    __solid = False;

    __sprite = "main.png" #default sprite

    def set_sprite(self, sprite): #all sprites are relative to sprites/.
        self.__sprite = sprite
        
    def get_sprite(self):
        return self.__sprite

    def is_solid(self):
        return self.__solid

    def set_solid(self, solid):
        self.__solid = solid

    def is_visible(self):
        return self.__visible

    def set_visible(self, visible):
        self.__visible = visible

    def __init__(self):
        print("Game_Object initiliasing")

    def before_frame_update(self):
        print("Woo Ice Cream!!")

    """ Returns the location of the player,
    the first coordinate is their x-axis position
    the second coordinate is their y-axis position
    the third coordinate is the layer they are in. """
    def get_location(self):
        return (12, 7, 0) #method stub



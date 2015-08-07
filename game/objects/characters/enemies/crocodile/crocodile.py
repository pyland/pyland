import sys
import os
sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/../../../characters')
#sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/../../../game_object')
#from game_object import GameObject
from character import Character #TODO: it should import from here, but some kind of definitions with the movement functions are screwing around with follow_path, causing the game to freeze.

import random

def hello_world():
    print("hello_world")


""" A brief description of the class will go here.
The auto-generated comment produced by the script should also mention where to get a list of the api
and which built-in variables exist already.
"""
class Crocodile(Character):

    """ constructor
    run when the object is created in-engine
    """
    def __init__(self):
        super().__init__()


    """ game engine features (public)
    These are methods which the game engine will execute at the commented moments.
    This will all be autofilled by the creation script with super filled in to help
    prevent errors when it comes to this.
    """

    """ This method is run every frame before the graphics are displayed.
    You can put code here you want to run before every frame.
    You MUST but super().beforeFrameUpdate() for this to work. Otherwise this may lead
    to unexpected behaviour. MAY NOT BE NEEDED, may be able to do everything with callbacks!
    """
    def before_frame_update(self):
        super().before_frame_update()
        self.__handle_movement_input()

    """ public:
    Put the regular public methods you wish to use here.
    """

    """ The Crocodile follows the path given, a string a comma-seperated directions, eg. "north, east, east, south, west, north"
    If repeat is set to True, the most recent direction completed will be added to the end of the string so that whole thing becomes a cycle
    """
    def follow_path(self, path, repeat = False):
        if(path.strip() == ""): #if path is empty terminate
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
                return self.wait(0.3, lambda: self.follow_path(old_path, repeat))
            else:
                return self.move_north(lambda: self.follow_path(path, repeat))
        elif(instruction == "east"):
            if engine.is_solid((x+1, y)): #if the position you are trying to move to is taken, wait
                self.face_east()
                return self.wait(0.3, lambda: self.follow_path(old_path, repeat))
            else:
                return self.move_east(lambda: self.follow_path(path, repeat))
            return
        elif(instruction == "south"):
            if engine.is_solid((x, y-1)):
                self.face_south()
                return self.wait(0.3, lambda: self.follow_path(old_path, repeat))
            else:
                return self.move_south(lambda: self.follow_path(path, repeat))
        elif(instruction == "west"):
            if engine.is_solid((x-1, y)):
                self.face_west()
                return self.wait(0.3, lambda: self.follow_path(old_path, repeat))
            else:
                return self.move_west(lambda: self.follow_path(path, repeat))
        else:
            pass #TODO: handle invalid path!!!!!
            print(instruction)
            print(path)

        return

    """ The Crocodile follows a random path forever!
    """
    def rand_explore(self):
        rand = random.randint(1, 5)
        if rand==1:
            return self.move_north(self.rand_explore)
        elif rand==2:
            return self.move_south(self.rand_explore)
        elif rand==3:
            return self.move_east(self.rand_explore)
        elif rand==4:
            return self.move_west(self.rand_explore)
        elif rand==5:
            return self.wait(0.3, self.rand_explore)

    """ private:
    Put the private methods you wish to use here.
    """









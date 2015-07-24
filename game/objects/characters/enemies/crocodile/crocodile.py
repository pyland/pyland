import sys
import os
#sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/../../../characters')
sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/../../../game_object')
from game_object import GameObject
#from character import Character TODO: it should import from here, but some kind of definitions with the movement functions are screwing around with follow_path, causing the game to freeze.

def hello_world():
    print("hello_world")


""" A brief description of the class will go here.
The auto-generated comment produced by the script should also mention where to get a list of the api
and which built-in variables exist already.
"""
class Crocodile(GameObject):
    
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

        comma_location = path.find(",") # Find the first comma in the path
        if(comma_location == -1):  # No commas in the path! On last word!
            comma_location = len(path)
        
        instruction = path[ 0 : comma_location].strip() #get instruction and remove whitespace
        path = path[comma_location + 1: ].strip() #remove the instruction from the path itself
        if(repeat):
            path = path + ", " + instruction #add instruction back to the path
        if(instruction == "north"):
            return self.move_north(lambda: self.follow_path(path, repeat))
        elif(instruction == "east"):
            self.move_east(lambda: self.follow_path(path, repeat))
            return
        elif(instruction == "south"):
            return self.move_south(lambda: self.follow_path(path, repeat))
        elif(instruction == "west"):
            return self.move_west(lambda: self.follow_path(path, repeat))
        else:
            pass #TODO: handle invalid path!!!!!
            print(instruction)
            print(path)
        
        return
        
        

    """ private:
    Put the private methods you wish to use here.
    """









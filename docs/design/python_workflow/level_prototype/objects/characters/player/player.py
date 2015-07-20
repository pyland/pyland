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
#__ import_object characters/Character
"""
The game code, at runtime, could recognise the "#__"
and replace it with:
"""
import sys
sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/../../characters')
from character import Character
"""
As the Character is in the characters folder.
"""


""" A brief description of the class will go here.
The auto-generated comment produced by the script should also mention where to get a list of the api
and which built-in variables exist already.
"""
class Player(Character):
    
    """ constructor
    run when the object is created in-engine
    """
    def __init__(self, name):
        super().__init__()
        #self.setSprite(), overwrite what could be grabbed from tiled
        print('I am alive!!!! I am ' + name) #Maybe have a debug option as well?
    
    """ game engine features (public)
    These are methods which the game engine will execute at the commented moments.
    This will all be autofilled by the creation script with super filled in to help
    prevent errors when it comes to this.
    """

    """ This method is run every frame before the graphics are displayed.
    You can put code here you want to run before every frame.
    You MUST but super().beforeFrameUpdate() for this to work. Otherwise this may lead
    to unexpected behaviour.
    """
    def before_frame_update(self):
        super().before_frame_update()
        self.__handle_movement_input()

    """ public:
    Put the regular public methods you wish to use here.
    """

    """ private:
    Put the private methods you wish to use here.
    """
    
    """ This method takes the movement input of the player character and appropriately
    handles it.
    """
    def __handle_movement_input(self):
        if(not(self.moving())):  #can't register input if the character is in the middle of moving
            (x, y, z) = self.get_location() #NB z is the layer number of the object
            if(is_input_pressed(CONST_UP)):
                if(not(self.is_facing("north"))):
                    self.faceNorth()
                else:
                    (x, y) = map(operator.add, (x, y),(0, 1))
                    if(is_location_free(x, y, z)):
                        self.move()
            elif(is_input_pressed(CONST_DOWN)):
                if(not(self.is_facing("south"))):
                    self.face_south()
                else:
                    (x, y) = map(operator.add, (x, y),(0, -1))
                    if(is_location_free(x, y, z)):
                        self.move_south()
            elif(is_input_pressed(CONST_RIGHT)):
                if(not(self.is_facing("east"))):
                    self.face_east()
                else:
                    (x, y) = map(operator.add, (x, y),(1, 0))
                    if(is_location_free(x, y, z)):
                        self.move_east()
            elif(is_input_pressed(CONST_LEFT)):
                if(not(self.is_facing("west"))):
                    self.face_west()
                else:
                    (x, y) = map(operator.add, (x, y),(-1, 0))
                    if(is_location_free(x, y, z)):
                        self.move()

test = Player("Larry")
test.before_frame_update()

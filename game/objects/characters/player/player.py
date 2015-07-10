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
        #register button callbacks
        api.add_button_callback(api.CONST_BUTTON_UP, self.__handle_movement_input(self.is_facing_north, self.face_north, self.move_north)
        api.add_button_callback(api.CONST_BUTTON_RIGHT, self.__handle_movement_input(self.is_facing_east, self.face_east, self.move_east)
        api.add_button_callback(api.CONST_BUTTON_DOWN, self.__handle_movement_input(self.is_facing_south, self.face_south, self.move_south)
        api.add_button_callback(api.CONST_BUTTON_LEFT, self.__handle_movement_input(self.is_facing_west, self.face_west, self.move_west)

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

    """ public:
    Put the regular public methods you wish to use here.
    """

    """ private:
    Put the private methods you wish to use here.
    """
    
    """ This method takes the movement input of the player character and returns the appropriate
    function for moving them in the direction required
    face_x -- self.face_north/east/south/west() as appropriately required to get them to face in that direction
    """
    def __handle_movement_input(self, is_facing_x, face_x, move_x):
        def handle_input:
            if(not(self.moving())):  #can't register input if the character is in the middle of moving
                if(is_facing_x()): #if facing in x direction, get them to move in that direction, else face in that direction first  
                    move_x()
                else:
                    face_x()
        return handle_input




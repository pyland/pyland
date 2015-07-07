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
#__ import_object game_object/GameObject
"""
The game code, at runtime, could recognise the "#__"
and replace it with:
"""
import sys
sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/../../game_object')
from game_object import GameObject
"""
As the GameObject is in the game_object folder.
"""


""" Whenever a player walks onto a portal, it sends them to the level set on it.
"""
class Portal(GameObject):

    __destination = ""
    __entrance = 0
    __transition = "normal"


    def set_destination(self, destination)
        self.__destination = destination

    def set_transition(self, transition)
        self.__transition = fade
    
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

        #get own location
        location = self.get_location()
        gameobjects = get_objects_at(location) #returns a list of objects at the location given
        for gameobject in gameobjects:
            if(type(gameobject) == Player) #if player is standing on portal
            level_change(self.get_destination(), self.get_entrance(), self.get_transition()) #change the level, enter by given entrance.

    """ public:
    Put the regular public methods you wish to use here.
    """

    """ private:
    Put the private methods you wish to use here.
    """

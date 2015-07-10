""" A brief description of the class will go here.
The auto-generated comment produced by the script should also mention where to get a list of the api
and which built-in variables exist already.
"""
class Crocodile(Character):
    
    """ constructor
    run when the object is created in-engine
    """
    def __init__(self, name):
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
    def follow_path(self, path, repeat):
        if(path.strip() == ""): #if path is empty terminate
            return
        instruction = path[ path.find(",") : ].strip() #get instruction and remove whitespace
        path = path[ 0 : path.find(",") ].strip() #remove the instruction from the path itself
        if(repeat):
            path = path + ", " + instruction #add instruction back to the path
        
        if(instruction == "north"):
            self.move_north(lambda: self.follow_path(path, repeat))
        elif(instruction == "east"):
            self.move_east(lambda: self.follow_path(path, repeat))
        elif(instruction == "south"):
            self.move_south(lambda: self.follow_path(path, repeat))
        elif(instruction == "west"):
            self.move_west(lambda: self.follow_path(path, repeat))
        else
            pass #TODO: handle invalid path!!!!!

        return
        
        

    """ private:
    Put the private methods you wish to use here.
    """









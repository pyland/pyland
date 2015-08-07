
class Engine:
    """ This class is a python wrapper for all the engine features that are exposed to the game.
    
    As some features are implemented in python and some in C++, this consolidates both.
    """
    #Represents the cplusplus engine
    __cpp_engine = None
    
    #A dictonary of all the game objects in the level (maps from object_id to object)
    __game_objects = dict()
    
    def __init__(self, cpp_engine):
        """ On initialisation, the cplusplus engine is passed to this class to enable it to access the api of the game.
        
        Parameters
        ----------
        cpp_engine : C++GameEngine
            Represents an instance of the C++ engein. All of whom's properties are inherited by Engine
        """
        self.__cpp_engine = cpp_engine
        #Use some magic trickery to give the Engine class all the methods of the C++GameEngine with their functionality
        engine_properties = [a for a in dir(self.__cpp_engine) if not a.startswith('__')]   #get all the engine properties with the magic and private methods filtered out
        for engine_property in engine_properties:                                           #loop over all the engine properties
            if not hasattr(self, engine_property):                                          #only add the property if the engine doesn't have something by that name
                setattr(self, engine_property, getattr(self.__cpp_engine, engine_property)) #set the all the properties of Engine to match cpp_engine.


    def register_game_object(self, game_object):
        """ Register a game object. 

        Parmeters
        ---------
        game_object : GameObject
            The game object you wish to register
        """
        self.__game_objects[game_object.get_id()] = game_object # associate each game_object with it's id

    def get_objects_at(self, position):
        """ Returns a list of all the objects at a given position
        
        Overrides the get_objects_at method inherited from cpp engine, as that version returns a list of object ids and this returns
        the actual instances.

        Parameters
        ----------
        position : 2-tuple of int
            The position you want to check for the objects. (x, y)

        Returns
        -------
        list of GameObject
            returns a list of the objects at the given position
        """
        x, y = position                                     #Extract the position x and y coordinates
        game_objects= list()                        #initialise the list that will be returned giving the objects at the position
        object_ids = self.__cpp_engine.get_objects_at(x, y) #get a list of the ids of all the objects at the given position from the game engine
        for object_id in object_ids:                        #iterate over all the object_ids and grab the object associated with each one.
            game_objects.append(self.__game_objects[object_id])
        
        #return a list of the objects at the given position
        return game_objects

    def is_solid(self, position):
        x, y = position                                     #Extract the position x and y coordinates
        return self.__cpp_engine.is_solid(x, y)

    def print_terminal(self, message, highlighted = False):
        """ print the given message to in-game terminal
        
        Parameters
        ----------
        message
            the message that you wish to print (python will attempt to convert it to a string before printing if possible using the str() method)
        highlighted : bool
            if true the message is printed red, else it printed in black
        """
        self.__cpp_engine.print_terminal(str(message), highlighted)



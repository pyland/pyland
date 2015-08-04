
class Engine:
    """ This class is a python wrapper for all the engine features that are exposed to the game.
    
    As some features are implemented in python and some in C++, this consolidates both.
    """
    
    #Represents the cplusplus engine
    __cpp_engine = None
    
    def __init__(self, cpp_engine):
        """ On initialisation, the cplusplus engine is passed to this class to enable it to access the api of the game.
        
        Parameters
        ----------
        cpp_engine : C++GameEngine
            Represents an instance of the C++ methods. All of whom's methods are inherited by Engine
        """
        self.__cpp_engine = cpp_engine
        
        #Use some magic trickery to give the Engine class all the methods of the C++GameEngine with their functionality
        engine_properties = [a for a in dir(engine) if not a.startswith('__')] #get all the engine properties with the magic and private methods filtered out
        for engine_property in engine_properties: #loop over all the engine properties
            setattr(self, engine_property, getattr(self.__cpp_engine, engine_property)) #set the all the properties of Engine to match cpp_engine.



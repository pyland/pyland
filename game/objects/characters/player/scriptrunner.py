import code
import sys
import io
import os
import contextlib
import time
import threading

""" This file contains all the implementation details of how player scripts are interpreted and run.
"""

""" This function runs the script provided in the argument in a seperate thread.
"""
def start(game_object, script_location):
    print("Running Player Script: " + script_location) #TODO: change this to some kind of script debug.
    #Creates 
    ScopedInterpreter = create_execution_scope(game_object) #Get the class definition for the ScopedIntepreter
    scoped_interpreter = ScopedInterpreter() #create an instance of it
    script_filename = os.path.dirname(os.path.realpath(__file__)) + "/" + script_location; #grab the absolute location of the script TODO: implement this path stuff in a config (ini) file!!!!!
    
    #open and print the script
    with open(script_filename, encoding="utf8") as script_file:
                script = script_file.read()
                print(script)

    #run the script using the scoped_intepreter in a new thread!
    threading.Thread(target = lambda: scoped_interpreter.runcode(script)).start()
    return

""" Takes an asynchronous function as an argument and returns a version of it that is blocking.
Works by using callbacks to work out when the function has finished.
Therefore the function must be a function which takes a callback as an argument which runs when event
that the function initiates has finished.
"""
def make_blocking(async_function):
    """ This callback is really simple, accepts a list
    and sets the first element to False, as the blocking_function
    is while loop that runs until the first element of a list is False,
    once this runs the while loop while finish and it will return.
    """
    def callback(blocking):
        print("callback called")
        blocking[0] = False
    
    """ This is the blocking version of the async_function that is provided as and argument.
    simples.
    """
    def blocking_function():
        blocking = [True] # create a list who's first member is true,
        async_function(lambda: callback(blocking)) #run the async_function with the callback provided above as its argument
        while blocking[0]: # run a while loop (block) until the first element of the list that was passed to the callback is false.
            pass #TODO: Find out if there is anything that needs to be done so that this doesn't use CPU time.
        return
    
    return blocking_function
    

""" TODO: CLEAN THIS UP, THIS UP A LOT (do the same in bootsrapper.py), this is a really really really
weird, and as far as I can see, unnesecary way of defining a class. But what do I know :P

Creates and defines a class called ScopedIntepreter which is used to run the player-code.
"""
def create_execution_scope(game_object): #""", RESTART, STOP, KILL"""
    """ imbued_locals is a python dictionary of python objects (variables, methods, class instances etc.)
    available to the player. :)
    eg. if there is an entry named "fart" whos entry is blob, then in the level script, any reference to fart
    will be refering to what blob is known as here.
    Here the list of game_objects is being looped through, and their names are being mapped to each instance :)
    """
    imbued_locals = {}

    # Provide all the movement functions to the player, but make them blocking.
    imbued_locals["move_north"] = make_blocking(game_object.move_north)
    imbued_locals["move_east"] = make_blocking(game_object.move_east)
    imbued_locals["move_south"] = make_blocking(game_object.move_south)
    imbued_locals["move_west"] = make_blocking(game_object.move_west)

    class ScopedInterpreter(code.InteractiveInterpreter):
        def __init__(self):
            super().__init__(imbued_locals)

        """ TODO: work out what this does, was in bootstrapper.py so assumed to be needed here!"""
        def write(self, data):
            print(data)

        def runcode(self, code):
            old_stdout = sys.stdout
            sys.stdout = io.TextIOWrapper(io.BytesIO(), sys.stdout.encoding)

            #"""TODO: work out how to catch exceptions safely and handle them with python,
            #then we can query the database to print output and errors messages cleanly to the user!"""
            #TODO: work out what the hell contextlib.closing does, don't forget to change bootstrapper.py based on that!
            try:
                with contextlib.closing(sys.stdout):
                    pass
                    # Emulate super().runcode(code), but don't catch
                    # RESTART, STOP or KILL because those should
                    # propogate to allow thread control.
                    #
                    # The code here is a trivial modification of the
                    # source of the implementaion we're overloading.
            
                    try:
                        exec(code, self.locals)
                    except (SystemExit): #, RESTART, STOP, KILL, TODO: work out what system_exit actually is!
                        raise
                    except:
                        self.showtraceback() # TODO: work out how this works, comment it in bootstrapper.py as well
                    # Read
                    sys.stdout.seek(0)
                    output = sys.stdout.read()
            
            finally:
                sys.stdout = old_stdout
            
            if output:
                """ This if statement is called whenever user calls "print(data)" inside their code,
                TODO: make it so that it prints the output to the player console! However, this version
                only prints to the console once the script has finished as opposed to printing in real time?
                """
                print(output)

    return ScopedInterpreter
    

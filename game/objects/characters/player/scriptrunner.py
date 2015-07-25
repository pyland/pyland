import code
import sys
import io
import os
import contextlib
import time
import threading

sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/../../../components')
from scoped_interpreter import ScopedInterpreter

""" This file contains all the implementation details of how player scripts are interpreted and run.
"""

""" This function runs the script provided in the argument in a seperate thread.
"""
def start(game_object, script_name):
    print("Running Player Script: " + script_name) #TODO: change this to some kind of script debug.

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

    
    #Creates 
    #ScopedInterpreter = create_execution_scope(game_object) #Get the class definition for the ScopedIntepreter
    scoped_interpreter = ScopedInterpreter(imbued_locals, print) #create an instance of it
    script_filename = os.path.dirname(os.path.realpath(__file__)) + "/../../../player_scripts/" + script_name + ".py"; #grab the absolute location of the script TODO: implement this path stuff in a config (ini) file!!!!!
    
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
    

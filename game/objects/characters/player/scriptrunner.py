import code
import sys
import io
import os
import contextlib
import time
import threading

#sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/../../../components')
from scoped_interpreter import ScopedInterpreter

""" This file contains all the implementation details of how player scripts are interpreted and run.
"""

class HaltScriptException(Exception):
    pass

""" This function runs the script provided in the argument in a seperate thread,
the script has access to a set of API's defined in imbued_locals that allow
it to control the player object provided.

the callback is run after the script has finished running.
player_object -- this is the instance of the player_object which is interacted with using the script.
script_name -- the name of the script that you wish to run
callback -- the callback function that will run once the script has completed  (in the same thead that the script was running in)
"""
def start(player_object, script_name):
    """ imbued_locals is a python dictionary of python objects (variables, methods, class instances etc.)
    available to the player. :)
    eg. if there is an entry named "fart" whos entry is blob, then in the level script, any reference to fart
    will be refering to what blob is known as here.
    Here the list of game_objects is being looped through, and their names are being mapped to each instance :)
    """
    imbued_locals = {}

    # Provide all the movement functions to the player, but make them blocking.
    imbued_locals["move_north"] = make_blocking(player_object.move_north)
    imbued_locals["move_east"] = make_blocking(player_object.move_east)
    imbued_locals["move_south"] = make_blocking(player_object.move_south)
    imbued_locals["move_west"] = make_blocking(player_object.move_west)

    #Replace print statement in player script so that all their output goes to the terminal.
    player_print = lambda text : player_object.get_engine().print_terminal(str(text) + "\n", False) #define the player printing function
    imbued_locals["print"] = player_print #autoconvert print to strings (do not need to convert within the game)
    
    imbued_locals["print_bag_items"] = lambda: player_print(player_object.bag_items_string())

    #Instantiate the scoped intepreter
    scoped_interpreter = ScopedInterpreter(imbued_locals, player_object.get_engine().print_terminal) #create an instance of it
    script_filename = os.path.dirname(os.path.realpath(__file__)) + "/../../../player_scripts/" + script_name + ".py"; #grab the absolute location of the script TODO: implement this path stuff in a config (ini) file!!!!!

    #open and read the script
    with open(script_filename, encoding="utf8") as script_file:
                script = script_file.read()

    def thread_target():
        """ this is the method that is run in the seperate thread,
        it runs the script requested first and then runs the callback.
        the callback is therefore run in the seperate thread.
        """
        try:
            scoped_interpreter.runcode(script) #Run the script
        except HaltScriptException: #If an exception is sent to halt the script, catch it and act appropriately
            player_object.get_engine().print_terminal("Halted Script\n", True)
        finally: #perform neccesary cleanup
            player_object.get_engine().print_terminal("-------------\n", False)
            player_object.set_running_script_status(False)
            #TODO: Make it so that the halt button becomes the run button again.
    
    thread = threading.Thread(target = thread_target, name = player_object.get_name() + "_script_thread")
    thread.start()
    player_object.set_thread_id(thread.ident)
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


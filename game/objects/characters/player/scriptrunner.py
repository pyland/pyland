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

class HaltScriptException(Exception):
    pass

def start(player_object, script_name):
    """ This function runs the script provided in the argument in a seperate thread.

    The script has access to a set of API's defined in imbued_locals that allow
    it to control the player object provided.

    the callback is run after the script has finished running.

    Parameters
    ----------
    player_object : Player
        This is the instance of the player_object which is interacted with using the script.
    script_name : str
        The name of the script that you wish to run. The game looks in the script folder for it.
    """


    #Imbued_locals is a python dictionary of python objects (variables, methods, class instances etc.)
    #available to the player. :)
    #eg. if there is an entry named "fart" whos entry is blob, then in the level script, any reference to fart
    #will be refering to what blob is known as here.
    #Here the list of game_objects is being looped through, and their names are being mapped to each instance :)
    imbued_locals = {}

    # Provide all the movement functions to the player, but make them blocking.
    imbued_locals["move_north"] = make_blocking(player_object.move_north)
    imbued_locals["move_east"] = make_blocking(player_object.move_east)
    imbued_locals["move_south"] = make_blocking(player_object.move_south)
    imbued_locals["move_west"] = make_blocking(player_object.move_west)


    #All the stuff to define new print stuff
    printed_flag = [False]
    def user_print(text):
        """ A simple mthod to print text to the game console for the user, overrides the python default print method """
        printed_flag[0] = True
        player_object.get_engine().print_terminal(text, False) #autoconvert print to strings (do not need to convert within the game)

    #Replace print statement in player script so that all their output goes to the terminal.
    imbued_locals["print"] = user_print
    imbued_locals["print_bag_items"] = lambda: user_print(player_object.bag_items_string())

    #the method to get the position of the player
    imbued_locals["get_position"] = player_object.get_position

    imbued_locals["test_display"] = player_object.test_display

    #Instantiate the scoped intepreter
    scoped_interpreter = ScopedInterpreter(imbued_locals, lambda x: player_object.get_engine().print_terminal(x, True)) #create an instance of it
    #script_filename = os.path.dirname(os.path.realpath(__file__)) + "/../../../player_scripts/Script " + script_name + ".py"; #grab the absolute location of the script TODO: implement this path stuff in a config (ini) file!!!!!
    script_filename = os.path.dirname(os.path.realpath(__file__)) + "/../../../player_scripts/"+str(script_name)+".py"; #grab the absolute location of the script TODO: implement this path stuff in a config (ini) file!!!!!

    #open and read the script
    with open(script_filename, encoding="utf8") as script_file:
                script = script_file.read()

    def thread_target():
        """ This is the method that is run in the seperate thread.

        It runs the script requested first and then runs the callback.
        the callback is therefore run in the seperate thread.
        """
        try:
            scoped_interpreter.runcode(script, HaltScriptException) #Run the script
        except HaltScriptException: #If an exception is sent to halt the script, catch it and act appropriately
            player_object.get_engine().print_terminal("Halted Script", True)
            player_object.get_engine().set_finished()
        finally: #perform neccesary cleanup
            if printed_flag[0]:
                player_object.get_engine().print_terminal("---" + player_object.get_character_name() + "'s script has ended---", False)
            player_object.set_running_script_status(False)
            player_object.get_engine().set_finished()
            #TODO: Make it so that the halt button becomes the run button again.

    thread = threading.Thread(target = thread_target, name = player_object.get_name() + "_script_thread")
    thread.start()
    player_object.set_thread_id(thread.ident)
    return

def make_blocking(async_function):
    """ Takes an asynchronous function as an argument and returns a version of it that is blocking.

    Works by using callbacks to work out when the function has finished.
    Therefore the function must be a function which takes a callback as an argument which runs when event
    that the function initiates has finished.
    """
    def blocking_function():
        """ This is the blocking version of the async_function that is provided as and argument. """
        lock = threading.Lock()
        lock.acquire()
        async_function(lock.release) #run the async_function with the callback provided above as its argument
        lock.acquire() # Try to a acquire a lock until it is released. It isn't released until the callback releases it.
        lock.release() # release the lock, it isn't needed anymore
        return

    return blocking_function


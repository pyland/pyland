import code
import sys
import io
import os
import contextlib
import time
import threading

from scoped_interpreter import ScopedInterpreter

""" This file contains all the implementation details of how player scripts are interpreted and run. """

class HaltScriptException(Exception):
    pass

def start(script_api, script_name, script_state_container, engine):
    """ This function runs the script provided in the argument in a seperate thread.

    The script has access to a set of API's defined in script_api that allow
    it to control the player object provided.

    the callback is run after the script has finished running.

    Parameters
    ----------
    script_state_container : ScriptStateContainer
        This is the instance of the script_state_container which is interacted with using the script.
    script_name : str
        The name of the script that you wish to run. The game looks in the script folder for it.
    """

    """ How printing is handled """
    printed_flag = [False]  #This flag used to determine if the player has printed something, because if nothing has been printed there is no point in inserting the "----" in the terminal.
    def user_print(text):
        """ A simple mthod to print text to the game console for the user, overrides the python default print method """
        printed_flag[0] = True
        engine.print_terminal(text, False) #autoconvert print to strings (do not need to convert within the game)

    #Replace print statement in player script so that all their output goes to the terminal. (unless it has already been overidden)
    if not "print" in script_api:
        script_api["print"] = user_print

    #Instantiate the scoped intepreter
    scoped_interpreter = ScopedInterpreter(script_api, lambda error_output: engine.print_terminal(error_output, True)) #create an instance of it
    script_filename = engine.get_config()['files']['player_scripts'] + "/"+str(script_name)+".py" #grab the absolute location of the script TODO: implement this path stuff in a config (ini) file!!!!!

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
            engine.print_terminal("Halted Script", True)
            printed_flag[0] = True
        finally:
            if printed_flag[0]:
                engine.print_terminal("---" + script_state_container.get_script_name() + "'s script has ended---", False)
            script_state_container.set_running_script_status(False)
            engine.set_finished()

    thread = threading.Thread(target = thread_target)
    thread.start()
    script_state_container.set_thread_id(thread.ident) #Save the player's thread id so that scripts can be halted
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


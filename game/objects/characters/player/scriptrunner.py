import code
import sys
import io
import os
import contextlib
import time
import multiprocessing
import threading

def start(game_object, script):
    print("testing player run scripts")
    ScopedInterpreter = create_execution_scope(game_object)
    scoped_interpreter = ScopedInterpreter()
    script_filename = os.path.dirname(os.path.realpath(__file__)) + "/" + script; #TODO: implement this path stuff in a config (ini) file!!!!!
    print(script_filename)
    print(script)
    with open(script_filename, encoding="utf8") as script_file:
                script = script_file.read()
                print(script)

    threading.Thread(target = lambda: scoped_interpreter.runcode(script)).start()

""" Takes an asynchronous function as an argument and returns a version of it that is blocking.
Works by using callbacks to work out when the function has finished.
Therefore the function must be a function which takes a callback as an argument which runs when event
that the function initiates has finished.
"""
def make_blocking(async_function):
    def callback(blocking):
        print("callback called")
        blocking[0] = False
    def blocking_function():
        blocking = [True]
        async_function(lambda: callback(blocking))
        while blocking[0]:
            pass
        return
    
    return blocking_function
    

def create_execution_scope(game_object): #""", RESTART, STOP, KILL"""
    """ imbued_locals is a python dictionary of python objects (variables, methods, class instances etc.)
    available to the player. :)
    eg. if there is an entry named "fart" whos entry is blob, then in the level script, any reference to fart
    will be refering to what blob is known as here.
    Here the list of game_objects is being looped through, and their names are being mapped to each instance :)
    """
    imbued_locals = {}
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
                    except (SystemExit): #, RESTART, STOP, KILL
                        raise
                    except:
                        self.showtraceback()
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
    

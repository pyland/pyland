import code
import os
import pydoc
import sys
import time
import threading
import traceback
import importlib

from contextlib import closing
from io import BytesIO, TextIOWrapper

def cast(cast_type, value):
    """
    A value → integer cast that is weaker than int(...) or float(...),
    primarily by not casting strings.

    Usage:
        cast("int", 32.1)
    """

    try:
        cast_method = getattr(value, "__{}__".format(cast_type))

    except AttributeError:
        raise TypeError("Value ({}) cannot be cast to {}".format(value, cast_type))

    return cast_method()

def create_execution_scope(game_objects, engine, RESTART, STOP, KILL):
    
    """ imbued_locals is a python dictionary of python objects (variables, methods, class instances etc.)
    eg. if there is an entry named "fart" whos entry is blob, then in the level script, any reference to fart
    will be refering to what blob is known as here.
    Here the list of game_objects is being looped through, and their names are being mapped to each instance :)
    """
    imbued_locals = {}
    for game_object in game_objects:
        imbued_locals[game_object.get_name()] = game_object

    class ScopedInterpreter(code.InteractiveInterpreter):
        def __init__(self):
            super().__init__(imbued_locals)

        def write(self, data):
            #entity.print_dialogue(data) # work out what to do here now! (Should print to in-game console)
            engine.print_debug(data)

        def runcode(self, code):
            old_stdout = sys.stdout
            sys.stdout = TextIOWrapper(BytesIO(), sys.stdout.encoding)

            try:
                with closing(sys.stdout):
                    # Emulate super().runcode(code), but don't catch
                    # RESTART, STOP or KILL because those should
                    # propogate to allow thread control.
                    #
                    # The code here is a trivial modification of the
                    # source of the implementaion we're overloading.

                    try:
                        exec(code, self.locals)
                    except (SystemExit, RESTART, STOP, KILL):
                        raise
                    except:
                        self.showtraceback()
                    # Read
                    sys.stdout.seek(0)
                    output = sys.stdout.read()

            finally:
                sys.stdout = old_stdout

            if output:
                engine.print_debug(output)
                #entity.print_dialogue(output)

    return ScopedInterpreter

""" converts snake_case to CamelCase """
def snake_to_camelcase(snake_string):
    components = snake_string.split('_')
    result = ""
    for component in components:
        result += component.title() #title automatically capitalises the first letter of a string! :D
    return result

""" Imports the correct files and class for each entity, and then wraps them in the correct game object
"""
def wrap_entity_in_game_object(entity):
    print("1")
    #Grabs the object's location in the file system (original data comes from the map's tmx file, eg. characters/enemies/crocodile
    #entity_location = entity.get_location()
    entity.callback_test(lambda: print("b"))
    entity_location = entity.get_location()
    print(entity_location)
    print("2")
    print(entity_location)
    print("3")
    #Imports the correct module based on that path name
    sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + "/../../../game/objects/" + entity_location)  # Go to the correct folder
    module_name = entity_location[entity_location.rfind("/") + 1: ] # Then get the name of the file itself (same name as the folder it's in, so can be extracted from the path name, eg crocodile/crocodile.py)
    module = importlib.import_module(module_name) # Import the module as module

    #Get the class from the module, will be the same as it's file but in UpperCamelCase eg. SuperCrocodile in super_crocodile.py
    wrapper_class = getattr(module, snake_to_camelcase(module_name))
    game_object = wrapper_class()  # create the object
    game_object.set_entity(entity)  # initialise it and wrap the entity instance in it
    return game_object
    

class DummyEngine:
    def get_level_location(self):
        return "test_world/test_level/test_one"
    def print_debug(self, string):
        print(string)
        return

def start(entities, RESTART, STOP, KILL, waiting, engine = DummyEngine()):
    """
    Run the main bootstrapper loop! It's fun!
    """
    engine.print_debug("Started bootstrapper")
    game_objects = list()
    print(entities)
    """Grab each entity in the entities list. Wrap them in the approperiate class :D (the classes defined in game)"""
    
    for entity in entities:
        print(entity.get_location())
        #game_object = wrap_entity_in_game_object(entity)
        #game_objects.append(game_object)
        #engine.print_debug("Converted entity {} to game_object {}".format(entity, game_object))
        #engine.print_debug("whose name is {}".format(game_object.get_name()))
    """
    ScopedInterpreter = create_execution_scope(game_objects, engine, RESTART, STOP, KILL)
    scoped_interpreter = ScopedInterpreter()
    
    while True:
        try:
            while waiting:
                # Smallest reasonable wait while
                # allowing fast interrupts.
                #
                # Could later be replaced with locking
                # for proper interrupts.
                time.sleep(0.05)

            script_filename = os.path.dirname(os.path.realpath(__file__)) + "/../../../game/levels/{}/scripts/start.py".format(engine.get_level_location()); #TODO: implement this path stuff in a config (ini) file!!!!!
            engine.print_debug("Reading from file: {}".format(script_filename))
            with open(script_filename, encoding="utf8") as script_file:
                script = script_file.read()
                engine.print_debug(script)


            #entity.update_status("running"), all the update status stuff are from old version of bootstrapper TODO: work out what this should change to

            scoped_interpreter.runcode(script)

            #entity.update_status("finished")

        except RESTART:
            engine.print_debug("restarting")

            waiting = False
            continue

        except STOP:
            engine.print_debug("STOPPING")
            #entity.update_status("stopped")
            waiting = True
            continue

        except KILL:
            engine.print_debug("KILLED")
            # Printing from Python when the game is dead hangs
            # everything, so don't do it.
            # TODO (Joshua): Fix this problem
            raise

        # For all other errors, output and stop
        except:
            waiting = True
            #entity.update_status("failed")
            engine.print_dialogue(traceback.format_exc());

        else:
            break
        """

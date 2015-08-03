import code
import os
import pydoc
import sys
import time
import threading
import traceback

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

def create_execution_scope(entity, RESTART, STOP, KILL):
    # Create all of the functions whilst they are
    # able to capture entity in their scope

    north = 0,+1
    south = 0,-1
    east = +1,0
    west = -1,0

    def cut(position):
        """
        Take a relative position (north, south, east or west)
        and attemt to cut any vines or other cuttable object at that position.
        """

        entity.print_debug("Python: cut({})".format(position))
        x, y = position
        x = cast("int", x)
        y = cast("int", y)

        entity.cut(x, y)

    # Typically one would use a no-argument sentinel, but I
    # don't want to confuse new users when introspecting, and
    # variadic arguments look less obviously confusing
    # (even if they're actually more complicated)
    def help(*arguments):
        """
        Get the help for any object.

        If called with no arguments, output help
        about the current task.
        """

        if arguments:
            try:
                [argument] = arguments
            except ValueError:
                raise TypeError("help takes at most one argument, {} given".format(len(arguments)))

            entity.print_terminal(pydoc.render_doc(argument, "Help on %s", renderer=pydoc.plaintext),False)
        else:
            entity.print_terminal(entity.get_instructions(),False)

    def get_retrace_steps():
        """
        Get a list of directions to move in that will undo all
        of the steps taken from the last checkpoint.
        """

        return entity.get_retrace_steps()

    def look(search_range: [(str, int, int)]):
        """
        Return a list of (name, x, y) tuples, representing the
        name and position of all of the objects within 10 squares
        of the calling character.
        """

        entity.print_debug("Python: look({})".format(search_range))
        return entity.look(search_range)

    def monologue():
        """
        Output information about the character to the text box.
        """

        entity.monologue()

    def move(position):


        x, y = position
        x = cast("int", x)
        y = cast("int", y)
        return entity.move(x, y)

    def move_east():
        return entity.move_east()

    def move_west():
        return entity.move_west()

    def move_south():
        return entity.move_south()

    def move_north():
        return entity.move_north()

    def walkable(position) -> bool:
        """
        Take a relative position (north, south, east or west)
        and return whether that position is walkable.
        """

        x, y = position
        x = cast("int", x)
        y = cast("int", y)

        return entity.walkable(x, y)

    def read_message():
        """
        Read a secret note and return any information it contains.
        """

        return entity.read_message()

    def _print_debug(text: str):
        """
        Output debugging information to the program's logging facilities.
        """

        entity._print_debug(text)

    def print_override(text):
        """
        Override print function so all text is not printed at the end.
        """

        entity.print_terminal(str(text)+"\n", False)

    imbued_locals = {

        "north": north,
        "south": south,
        "east": east,
        "west": west,

        "cut": cut,
        "help": help,
        "get_retrace_steps": get_retrace_steps,
        "look": look,
        "move": move,
        "move_east": move_east,
        "move_north": move_north,
        "move_west": move_west,
        "move_south": move_south,
        "monologue": monologue,
        "read_message": read_message,
        "walkable": walkable,

        "print": print_override,
        "_print_debug": _print_debug
    }

    class ScopedInterpreter(code.InteractiveInterpreter):
        def __init__(self):
            super().__init__(imbued_locals)

        def write(self, data):
            entity.print_terminal(data,True)

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
                entity.print_terminal(output,False)

    return ScopedInterpreter

def start(entity, RESTART, STOP, KILL, waiting):
    """
    Run the main bootstrapper loop! It's fun!
    """

    entity.print_debug("Started bootstrapper")
    entity.print_debug("Started with entity {}".format(entity))
    entity.print_debug("whose name is {}".format(entity.name))

    ScopedInterpreter = create_execution_scope(entity, RESTART, STOP, KILL)
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

            #Read from current script before new input manager is implemented
            #to read the correct script
            script_filename = "python_embed/scripts/Current Script.py";
            entity.print_debug("Reading from file: {}".format(script_filename))

            with open(script_filename, encoding="utf8") as script_file:
                script = script_file.read()
                entity.print_debug(script)


            entity.update_status("running")

            scoped_interpreter.runcode(script)

            entity.update_status("finished")

        except RESTART:
            entity.print_debug("restarting")

            waiting = False
            continue

        except STOP:
            entity.print_debug("STOPPING")
            entity.update_status("stopped")
            waiting = True
            continue

        except KILL:
            entity.print_debug("KILLED")
            # Printing from Python when the game is dead hangs
            # everything, so don't do it.
            # TODO (Joshua): Fix this problem
            raise

        # For all other errors, output and stop
        except:
            waiting = True
            entity.update_status("failed")
            entity.print_terminal(traceback.format_exc(),True);

        else:
            break

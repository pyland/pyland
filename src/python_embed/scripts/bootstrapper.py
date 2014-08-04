import os
import time
import threading
import traceback

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

def create_execution_scope(entity):
    # Create all of the functions whilst they are
    # able to capture entity in their scope

    north =  0, +1
    south =  0, -1
    east  = +1,  0
    west  = -1,  0

    def move(position):
        entity.print_debug("Python: move({})".format(position))

        x, y = position
        x = cast("int", x)
        y = cast("int", y)

        entity.move(x, y)

    def monologue(*args):
        entity.print_debug("Python: monologue({})".format(args))
        entity.monologue("\n".join(args))

    def walkable(position):
        entity.print_debug("Python: walkable({})".format(position))

        x, y = position
        x = cast("int", x)
        y = cast("int", y)

        return entity.walkable(x, y)


    # Finally, export the desired behaviour
    return {
        "north": north,
        "south": south,
        "east":  east,
        "west":  west,

        "move": move,
        "monologue": monologue,
        "walkable": walkable
    }


def start(entity, RESTART, STOP, KILL, waiting):
    """
    Run the main bootstrapper loop! It's fun!
    """

    entity.print_debug("Started bootstrapper")
    print("Started with entity", entity)
    print("whose name is", entity.name)

    while True:
        try:
            while waiting:
                # Smallest reasonable wait while
                # allowing fast interrupts.
                #
                # Could later be replaced with locking
                # for proper interrupts.
                time.sleep(0.05)

            script_filename = "python_embed/scripts/John_{}.py".format(entity.id)
            entity.print_debug("Reading from file: {}".format(script_filename))

            with open(script_filename, encoding="utf-8") as script_file:
                script = script_file.read()
                entity.print_debug(script)

            execution_scope = create_execution_scope(entity)

            entity.__set_game_speed(100);
            try:
                exec(script, execution_scope)
            finally:
                entity.__set_game_speed(1);

        except RESTART:
            entity.print_debug("RESTARTING")

            waiting = False
            continue

        except STOP:
            entity.print_debug("STOPPING")

            waiting = True
            continue

        except KILL:
            entity.print_debug("DYING")

            raise

        # For all other errors, output and stop
        except:
            waiting = True
            entity.print_dialogue(traceback.format_exc());

        else:
            break

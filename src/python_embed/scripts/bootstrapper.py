import os
import time

def start(entity, RESTART, STOP, KILL, waiting):

    entity.print_debug("Started bootstrapper")
    print("Started with entity", entity)
    print("whose name is", entity.name)

    while True:
        try:
            while waiting:
                time.sleep(0.05)

            file_name = "python_embed/scripts/John_1.py"
            entity.print_debug ("reading from file :"+file_name)
            with open(file_name) as file:
                with open("python_embed/py_wrapper.py") as file_wrapper:
                    text = file.read()
                    entity.print_debug(text)
                    function = file_wrapper.read() + text

            exec(function, dict(entity=entity, **globals()))

        # TODO: These should be more thought-through.

        except RESTART:
            print("RESTARTING")
            waiting = False
            continue
        
        except STOP:
            print("STOPPING")
            waiting = True
            continue

        except KILL:
            print("DYING")
            raise

        else:
            break

# def run(entity):
#     """Purely for testing"""

#     print("Running with entity", entity)

#     for _ in range(500):
#         time.sleep(0.1)

#         import random
#         entity.move(random.randint(-1, 1), random.randint(-1, 1))
#         print("Continuing with entity", entity)

#     print("Finishing with entity", entity)

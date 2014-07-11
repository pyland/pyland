import time
import os

print("Started bootstrapper")

def start(entity):
    print (os.getcwd())
    print("Started with entity", entity)
    with open("python_embed/scripts/{}.py".format(entity.name)) as file:
        function = file.read()
        print (function)
    try:
        run(entity)
    except BaseException as e:
        print(e)
        raise

def run(entity):
    """Purely for testing"""

    print("Running with entity", entity)

    for _ in range(500):
        time.sleep(0.1)

        import random
        entity.move(random.randint(-1, 1), random.randint(-1, 1))
        print("Continuing with entity", entity)

    print("Finishing with entity", entity)

import time
import os

print("Started bootstrapper")

def start(entity):
    print("Started with entity", entity)
    print("whose name is", entity.name)

    with open("python_embed/scripts/{}.py".format(entity.name)) as file:
        with open("python_embed/py_wrapper.py") as file_wrapper:
            function = file_wrapper.read() + file.read()
            print (function)

    try:
        exec(function, dict(entity=entity, **globals()))
    except BaseException as e:
        print(e)
        raise

# def run(entity):
#     """Purely for testing"""

#     print("Running with entity", entity)

#     for _ in range(500):
#         time.sleep(0.1)

#         import random
#         entity.move(random.randint(-1, 1), random.randint(-1, 1))
#         print("Continuing with entity", entity)

#     print("Finishing with entity", entity)

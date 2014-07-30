"""Python wrapper for user scripts """

import time

def fun_wrapper (func, delay, args, kwargs):
    """functing wrapping to add delay & check number of arguments """
    if delay:
        time.sleep(0.04)
    # TODO: extend this for type checking and providing documentation
    if len(args) == 0:
        return func
    elif len(args) == 1:
        return func (*args[0])

# python API as exposed to the the user

def move(*args, **kwargs):
    return fun_wrapper(entity.move, True, args, kwargs)
    
def monologue(*args, **kwargs):
    return fun_wrapper(entity.monologue, False, args, kwargs)

def walkable(*args, **kwargs):
    return fun_wrapper(entity.walkable, False, args, kwargs)

north, south, east, west = (0, 1), (0, -1), (1, 0), (-1, 0)    

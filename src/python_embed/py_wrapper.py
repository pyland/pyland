import time
import inspect

def fun_wrapper (func, delay, args, kwargs):
	if delay:
		time.sleep(0.1)
	if len(args) == 0:
		return func
	elif len(args) == 1:
		return func (args[0])


def move(*args, **kwargs):
    return fun_wrapper(player.move, True, args, kwargs)
    
def monologue(*args, **kwargs):
    return fun_wrapper(player.monologue, False, args, kwargs)

def walkable(*args, **kwargs):
	return fun_wrapper(player.walkable, False, args, kwargs)

north, south, east, west = Vec2D(0, 1), Vec2D(0, -1), Vec2D(1, 0), Vec2D(-1, 0)    
def script(player):
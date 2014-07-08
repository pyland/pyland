import time
import inspect

def fun_wrapper (func, delay, args):
	if delay:
		time.sleep(0.1)
	if len(args) == 0:
		return func
	elif len(args) == 1:
		return func (args[0])


def move(x):
    return fun_wrapper(player.move, True, (x,))
    
def monologue():
    return fun_wrapper(player.monologue, False, ())

def walkable(x):
	return fun_wrapper(player.walkable, False, (x,))

north, south, east, west = Vec2D(0, 1), Vec2D(0, -1), Vec2D(1, 0), Vec2D(-1, 0)    
def script(player):
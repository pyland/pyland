import time

def move(x):
    player.move(x)
    time.sleep(0.1)
    return True
    
def monologue():
    player.monologue()

def walkable(x):
	return (player.walkable(x))

north, south, east, west = Vec2D(0, 1), Vec2D(0, -1), Vec2D(1, 0), Vec2D(-1, 0)    
def script(player):
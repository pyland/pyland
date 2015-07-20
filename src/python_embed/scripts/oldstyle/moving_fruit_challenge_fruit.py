#
# FRUIT
# Finnian Anderson (fxapi.co.uk)
#

import math
from random import randint
fruits = {}

def basket():
    return {"x": object_basket.x, "y": object_basket.y}

def move_fruit(object, x, y)
    me = {"x": me.x, "y": me.y}
    if me.x - basket.x < 0:
                try:
                        move_west()
                except:
            pass
    if me.x - basket.x > 0:
                try:
                        move_east()
                except:
            pass
    if me.y - basket.y < 0:
                try:
                        move_south()
                except:
            pass
        if me.y - basket.y > 0:
        try:	
            move_north()
        except:
            pass
    else:
        while move(randint(0,1),randint(0,1)) != True:
            move(randint(0,1),randint(0,1))

while True:
    move_fruit()

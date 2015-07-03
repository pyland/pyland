#
# MONKEY
# Finnian Anderson (fxapi.co.uk)
#

import math
from random import randint
fruits = {}

def take_action():
	seen_objects = look(radius = 5)
	if seen_objects is not None:
		for object in seen_objects:
			if object.type == "player":
                        	move_away_from(object.name, object.x, object.y)
	        for object in seen_objects:
                        if object.name == "banana": # priority
                                move_towards(object.name, object.x, object.y)
		for object in seen_objects:
			if object.type == "fruit":
				object.name = str(object.name)
				fruits[object.name] = [math.sqrt((object.x - monkey.x)^2 + (object.y - monkey.y)^2), object.x, object.y]
				sorted_fruits = sorted(fruits.iteritems, key=operator.itemgetter(1))
				return sorted_fruits[0]
	else:
		print("Saw nothing!")

def basket():
	return {"x": object_basket.x, "y": object_basket.y}

def move_towards(object, x, y)
	me = {"x": me.x, "y": me.y} # will always be fruit
	if me.x - object.x < 0:
                try:
                        move_west()
                except:
			pass
	if me.x - object.x > 0:
                try:
                        move_east()
                except:
			pass
	if me.y - object.y < 0:
                try:
                        move_south()
                except:
			pass
        if me.y - object.y > 0:
		try:	
			move_north()
		except:
			pass
	else:
		while move(randint(0,1),randint(0,1)) != True:
			move(randint(0,1),randint(0,1))

def move_away_from(object, x, y)
        me = {"x": me.x, "y": me.y} # will always be a player
        if me.x - object.x > 0:
                try:
                        move_west()
                except:
                        pass
        if me.x - object.x < 0:
                try:
                        move_east()
                except:
                        pass
        if me.y - object.y > 0:
                try:
                        move_south()
                except:
                        pass
        if me.y - object.y < 0:
                try:
                        move_north()
                except:
                        pass
        else:
                while move(randint(0,1),randint(0,1)) != True:
                        move(randint(0,1),randint(0,1))

while True:
	take_action()

# TODO: take action when monkey eats fruit = get quicker

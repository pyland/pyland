import random

direction = random.choice([north, south, east, west])

while True:
	direction = random.choice([north, south, east, west] + [direction] * 6)
	move(direction)

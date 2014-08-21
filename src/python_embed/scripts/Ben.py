from itertools import cycle

directions = cycle([north, east, south, west])
for direction in directions:
	while move(direction):
			pass

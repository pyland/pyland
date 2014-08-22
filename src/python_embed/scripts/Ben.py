"""
This is a wall-following script written in mostly idiomatic Python.

This could be used to solve mazes - potentially even very large ones,
but may fail if starting in the wrong place on a maze with loops.
"""

# A mapping that translates a direction 90° clockwise
rotate_cw = {
	north: east,
	east:  south,
	south: west,
	west:  north
}

# Invert this mapping by switching the keys and values
rotate_ccw = {v: k for k, v in rotate_cw.items()}

# Get to north wall
while walkable(north):
	move(north)

# Start going along the north wall,
# walking eastward.
direction = east

# Don't stop
while True:
	# If the direction of the wall is walkable,
	# you have just passed the end, so turn into it.
	if walkable(rotate_ccw[direction]):
		direction = rotate_ccw[direction]

	# If the direction of travel is blocked, turn
	# away from the wall.
	while not walkable(direction):
		direction = rotate_cw[direction]

	# Walk forward
	move(direction)

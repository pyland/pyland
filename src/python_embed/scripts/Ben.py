from itertools import cycle

rotate_cw = {
	north: east,
	east: south,
	south: west,
	west: north
}

rotate_ccw = {v: k for k, v in rotate_cw.items()}

# Get to north wall
while move(north):
	pass

wall = north

while True:
	if walkable(wall):
		wall = rotate_ccw[wall]

	if not walkable(rotate_cw[wall]):
		wall = rotate_cw[wall]

	move(rotate_cw[wall])

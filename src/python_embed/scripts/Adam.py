import random

direction = random.choice([north, south, east, west])

while True:
    direction = random.choice([north, south, east, west])
    if walkable(*direction):
        entity.move(*direction)
    else:
        monologue()
        print("lets not go there")

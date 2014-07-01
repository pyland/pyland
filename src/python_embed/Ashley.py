import random
move(north)
print("ASHLEY!!")
for _ in range(2400):
    move(random.choice([north, south, east, west]))
monologue()

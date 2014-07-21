import random

while True:
    while(walkable(north)):
        move(north)
    while(walkable(south)):
          move(south)



'''

import random

while True:
    move(random.choice([north, south, east, west]))

'''

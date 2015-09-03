import time

for i in range(26):
  move_west()
  
for i in range(5):
  move_north()
  
move_east()
yell()
move_south()
move_east()
yell()
move_south()
move_east()
yell()
move_south()
move_south()

for i in range(7):
  move_east()

for i in range(5):
  move_north()
move_east()
yell()

time.sleep(.5)
for i in range(5):
  move_south()
  
for i in range(17):
  move_east()
  
for i in range(6):
  move_north()
  
move_west()
yell()

move_north()

for i in range(5):
  move_west()
  
move_south()
time.sleep(.25)
yell()

for i in range(5):
  move_east()
  
time.sleep(.5)

for i in range(7):
  move_south()
  
for i in range(17):    
  move_west()
  
for i in range(7):
  move_north()
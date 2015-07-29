for _ in range(15):
  for _ in range(10):
    move_east()
  move_south()
  for _ in range(10):
    move_west()
  move_south()
for _ in range(30):
  move_north()
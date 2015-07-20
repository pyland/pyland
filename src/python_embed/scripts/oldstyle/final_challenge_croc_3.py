def move_croc(dx, dy):
    """This moves the croc"""

    if dx >  0:
        for x in range(dx):
            move_east()
    elif dx < 0:
        for x in range(-dx):
            move_west()
    #don't move the player if dx = 0

    if dy > 0:
        for y in range(dy):
            move_north()
    elif dy < 0:
        for y in range(-dy):
            move_south()
    #don't move the player if dy = 0



while True:
    move_croc(-7, 0)
    move_croc(0, 4)
    move_croc(3, 0)
    move_croc(0, 4)
    move_croc(-8, 0)
    move_croc(10, 0)
    move_croc(0, 8)
    move_croc(0, -8)
    move_croc(-2,0)
    move_croc(0, -4)
    move_croc(4, 0)
    move_croc(0, -4)



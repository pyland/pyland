player_one.focus()

b1_croc = [
croc_one,
croc_two,
croc_three,
croc_four,
croc_five,
croc_six,
croc_seven,
croc_eight,
croc_nine,
croc_ten]

b2_croc = [
b2_croc_1,
b2_croc_2,
b2_croc_3,
b2_croc_4,
b2_croc_5,
b2_croc_6,
b2_croc_7,
b2_croc_8,
b2_croc_9,
b2_croc_10,
b2_croc_11,
b2_croc_12,
b2_croc_13,
b2_croc_14,
b2_croc_15,
b2_croc_16,
b2_croc_17,
b2_croc_18,
]

bridge1_x = 25

def move_left(croc):
    x,y = croc.get_position()
    if engine.get_tile_type((x-1,y)) == engine.TILE_TYPE_WATER: #we are headed towards water
        if player_one.is_moving():
            if player_one.is_facing_north() and player_one.get_position() == (bridge1_x, y-1):
                engine.print_terminal("YOU LOST")
                player_one.set_busy(True)
                croc.move_to((bridge1_x+1, y), 0.4)
            else:
                croc.move_west(lambda: move_left(croc))
        else:
            if player_one.get_position() == (bridge1_x, y):
                engine.print_terminal("YOU LOST")
                player_one.set_busy(True)
                croc.move_to((bridge1_x+1, y), 0.4)
            else:
                croc.move_west(lambda: move_left(croc))
    else: #we are facing a bridge
        if player_one.is_moving():
            if player_one.is_facing_north() and player_one.get_position() == (bridge1_x, y-1):
                engine.print_terminal("YOU LOST")
                player_one.set_busy(True)
                croc.move_to((bridge1_x+1, y), 0.4)
            else:
                croc.face_east(lambda: move_right(croc))
        else:
            if player_one.get_position() == (38, y):
                engine.print_terminal("YOU LOST")
                player_one.set_busy(True)
                croc.move_to((bridge1_x+1, y), 0.4)
            else:
                croc.face_east(lambda: move_right(croc))
def move_right(croc):
    x,y = croc.get_position()
    if engine.is_solid((x+1,y)):
        croc.face_west(lambda: move_left(croc))
    else:
        croc.move_east(lambda: move_right(croc))


for croc in b1_croc:
    move_left(croc)

def face_south(croc):
    croc.face_south(lambda: croc.wait(.3, lambda: face_north(croc)))

def face_north(croc):
    croc.face_north(lambda: croc.wait(.3, lambda: face_south(croc)))

for i in range(len(b2_croc)):
    if i%2 == 0:
        face_south(b2_croc[i])
    else:
        face_north(b2_croc[i])
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
b2_croc_0,
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
b2_croc_17
]

b2_trigger = [
t0,
t1,
t2,
t3,
t4,
t5,
t6,
t7,
t8,
t9,
t10,
t11,
t12,
t13,
t14,
t15,
t16,
t17
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

def croc_toggle(croc):
    if croc.is_facing_north():
            croc.face_south()
    else:
        croc.face_north()

#t1.player_walked_on = lambda player_object: croc_toggle(b2_croc[0])

for i in range(18):
    b2_trigger[i].player_walked_on = lambda player_object: croc_toggle(b2_croc[i])
    #engine.print_terminal(i)
    #b2_trigger[b].player_walked_on = lambda player_object: engine.print_terminal(i)

engine.print_terminal(i)

player_one.focus()



intro_dialogue = [
    lambda callback: player_one.set_busy(True, callback = callback),
    lambda callback: engine.show_dialogue("Myla: The desert is on the other side of the river!", callback = callback),
    lambda callback: engine.show_dialogue("Myla: Careful, those crocodiles will eat you if they see you!", callback = callback),
    lambda callback: player_one.set_busy(False, callback =callback)
    ]



engine.run_callback_list_sequence(intro_dialogue)












b1_croc = [
croc_0,
croc_1,
croc_2,
croc_3,
croc_4,
croc_5,
croc_6,
croc_7,
croc_8,
croc_9]

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


######################################################################################################################
#Crocodiles are oscillating


def b2_move(croc):
    x,y = croc.get_position()
    if(croc.is_facing_west()):
        if not engine.get_tile_type((x-1,y)) == engine.TILE_TYPE_WATER:
            if (myla in engine.get_objects_at((x-1, y))):
                engine.print_terminal("THEY GOT MYLA")
            else:
                croc.face_east(lambda: b2_move(croc))
        else:
            croc.move_west(lambda: b2_move(croc))
    elif(croc.is_facing_east()):
        if not engine.get_tile_type((x+1,y)) == engine.TILE_TYPE_WATER:
            croc.face_west(lambda: b2_move(croc))
        else:
            croc.move_east(lambda: b2_move(croc))

b2_croc = [
croc_10,
croc_11,
croc_12,
croc_13,
croc_14,
croc_15,
croc_16,
croc_17,
croc_18,
croc_19,
croc_20,
croc_21
]

for croc in b2_croc:
    croc.face_west()
    b2_move(croc)

def yell():
    x,y = player_one.get_position()
    b2_croc[12-y].face_east()

player_one.yell = yell

myla.face_south()

def myla_follow():
    player_x,player_y = player_one.get_position()
    myla_x, myla_y = myla.get_position()

    if(myla_y < player_y):
        myla.move_north(lambda: myla_follow)

    if(myla_y > player_y):
        myla.move_south(lambda: myla_follow)


def player_walked_on():
    engine.register_input_callback(engine.INPUT_UP, myla_follow)
    engine.register_input_callback(engine.INPUT_DOWN, myla_follow)

challenge2.player_walked_on = lambda player_object: player_walked_on()
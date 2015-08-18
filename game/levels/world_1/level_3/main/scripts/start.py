player_one.focus()

all_croc = [
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
#croc_eleven]

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


for croc in all_croc:
    move_left(croc)

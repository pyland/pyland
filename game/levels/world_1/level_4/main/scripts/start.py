player_one.focus()

croc_0.change_state("swim")
croc_1.change_state("swim")
croc_2.change_state("swim")
croc_3.change_state("swim")
croc_4.change_state("swim")
croc_5.change_state("swim")

croc_0.face_east()
croc_1.face_east()
croc_2.face_east()

croc = [
croc_0,
croc_1,
croc_2,
croc_3,
croc_4,
croc_5]

croc[0].face_east()
croc[1].face_east()
croc[2].face_east()
croc[3].face_east()
croc[4].face_south()
croc[5].face_west()

triggers = [trigger_0,
trigger_1,
trigger_2,
trigger_3,
trigger_4,
trigger_5,
trigger_6,
trigger_7]


def player_walked_on_ti():
    x,y = player_one.get_position()
    for a in [-1, 1]:
        for obj in engine.get_objects_at((x+a,y)):
            if obj in croc:
                engine.print_terminal("you lose")
                return
        for obj in engine.get_objects_at((x,y+a)):
            if obj in croc:
                engine.print_terminal("you lose")
                return

for t in triggers:
    t.player_walked_on = lambda player_object: player_walked_on_ti()

def move_horizontal(croc, times = 100000):
    if times != 0:
        x,y = croc.get_position()
        if(croc.is_facing_west()):
            if not engine.get_tile_type((x-1,y)) == engine.TILE_TYPE_WATER:
                    return croc.face_east(lambda: move_horizontal(croc, times-1))
            else:
                return croc.move_west(lambda: move_horizontal(croc, times))
        elif(croc.is_facing_east()):
            if not engine.get_tile_type((x+1,y)) == engine.TILE_TYPE_WATER:
                    return move_horizontal(croc, times-1)
            else:
                    return move_horizontal(croc, times)

def move_vertical(croc, times = 100000):
    if times != 0:
        x,y = croc.get_position()
        if(croc.is_facing_north()):
            if not engine.get_tile_type((x,y+1)) == engine.TILE_TYPE_WATER:
                    return croc.face_south(lambda: move_vertical(croc, times-1))
            else:
                return croc.move_north(lambda: move_vertical(croc, times))
        elif(croc.is_facing_south()):
            if not engine.get_tile_type((x,y-1)) == engine.TILE_TYPE_WATER:
                    return croc.face_north(lambda: move_vertical(croc, times-1))
            else:
                return croc.move_south(lambda: move_vertical(croc, times))

def yell():
    x,y = player_one.get_position()
    engine.print_terminal("You yell!")

    for current in croc:
        croc_x, croc_y = current.get_position()
        c = current
        if y == croc_y:
            if x < croc_x:
                c.face_west(lambda: move_horizontal(c))
            elif x > croc_x:
                c.face_east(lambda: move_horizontal(c))
        if x == croc_x:
            if y < croc_y:
                c.face_south(lambda: move vertical(c))
            elif y > croc_y:
                c.face_north(lambda: move_vertical(c))




player_one.yell = yell







player_one.focus()
engine.play_music("beach")
level_name = "level_1"
world_name = "world_3"
map_name = "Downstream"

player_data.load(engine.get_player_name())
player_data.set_map(world_name, level_name = level_name, map_name = map_name) #change the map and save that the map has changed

intro_dialogue = [
    lambda callback: player_one.set_busy(True, callback = callback),
    lambda callback: myla.move_west(callback = callback),
    lambda callback: myla.move_west(callback = callback),
    lambda callback: myla.move_west(callback = callback),
    lambda callback: myla.move_west(callback = callback),
    lambda callback: myla.move_west(callback = callback),
    lambda callback: myla.move_west(callback = callback),
    lambda callback: myla.move_west(callback = callback),
    lambda callback: player_one.face_east(callback = callback),
    lambda callback: engine.show_dialogue("Myla: We need to go west, but the boulder is blocking us!", callback = callback),
    lambda callback: engine.show_dialogue("Careful, those crocodiles will eat you if they see you!", callback = callback),
    lambda callback: engine.show_dialogue("Since there is only one bridge, I'll just jump into your bag", callback =callback),
    lambda callback: myla.move_to((0,0), 0.0001, callback = callback),
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
        if player_one.get_position() == (bridge1_x, y):
            player_one.set_busy(True)
            croc.move_to((bridge1_x+1, y), 0.4, lambda: engine.run_callback_list_sequence([
                 lambda callback: engine.show_dialogue("Crocodile: I've got you!", callback = callback),
                ]))
        else:
            croc.move_west(lambda: move_left(croc))
    else: #we are facing a bridge
        if player_one.get_position() == (38, y):
            player_one.set_busy(True)
            croc.move_to((bridge1_x+1, y), 0.4, lambda: engine.run_callback_list_sequence([
                 lambda callback: engine.show_dialogue("Crocodile: I've got you!", callback = callback),
                ]))
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

#############################################################################################################################################################

def b2_move(croc):
    x,y = croc.get_position()
    if(croc.is_facing_west()):
        if not engine.get_tile_type((x-1,y)) == engine.TILE_TYPE_WATER:
            if (myla in engine.get_objects_at((x-1, y))):
                player_one.set_busy(True)
                engine.run_callback_list_sequence([
                     lambda callback: engine.show_dialogue("Crocodile: Monkey meat for dinner!", callback = callback),
                    ])
            else:
                return croc.face_east(lambda: b2_move(croc))
        else:
            return croc.move_west(lambda: b2_move(croc))
    elif(croc.is_facing_east()):
        if not engine.get_tile_type((x+1,y)) == engine.TILE_TYPE_WATER:
            if player_one in engine.get_objects_at((x+1,y)):
                player_one.set_busy(True)
                engine.run_callback_list_sequence([
                     lambda callback: engine.show_dialogue("Crocodile: I've got you!", callback = callback),
                    ])

            else:
                return croc.face_west(lambda: b2_move(croc))
        else:
            return croc.move_east(lambda: b2_move(croc))

b2_croc = [croc_b2_0,
croc_b2_1,
croc_b2_2,
croc_b2_3,
croc_b2_4,
croc_b2_5,
croc_b2_6,
croc_b2_7,
croc_b2_8,
croc_b2_9,
croc_b2_10,
croc_b2_11]


for croc in b2_croc:
    i = random.randint(1, 2)
    if i == 1:
        croc.face_west()
    else:
        croc.face_east()
    b2_move(croc)


def yell():
    x,y = player_one.get_position()
    engine.print_terminal("You yell!")
    if y in range (1, 13):
        b2_croc[12-y].face_east()
player_one.yell = yell

myla.face_south()

myla_follow = True
def follow(char):
    global myla_follow

    if myla_follow:
        engine.print_terminal("following")
        player_x,player_y = player_one.get_position()
        char_x, char_y = char.get_position()
        engine.print_terminal(player_x)
        engine.print_terminal(player_y)

        if(char_y < player_y):
            return char.face_north(lambda: char.move_north(lambda: follow(char)))

        if(char_y > player_y):
            return char.face_south(lambda: char.move_south(lambda: follow(char)))


first_time_a = True
first_time_b = True
def player_walked_on_challenge2a():
    global first_time_a;
    if first_time_a:
        first_time_a = False
        challenge2_dialogue = [
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("Myla: Good job!", callback = callback),
        lambda callback: engine.show_dialogue("Your backpack is getting a bit stuffy, let me out!", callback = callback),
        lambda callback: myla.move_to((17,13), 0.0001, callback = callback),
        lambda callback: myla.face_east(callback = callback),
        lambda callback: engine.show_dialogue("This bridge doesn't look sturdy enough to support both of us...", callback = callback),
        lambda callback: engine.show_dialogue("I'll just use that bridge over there", callback = callback),
        lambda callback: myla.move_west(callback = callback),
        lambda callback: myla.move_west(callback = callback),
        lambda callback: myla.move_west(callback = callback),
        lambda callback: myla.move_west(callback = callback),
        lambda callback: myla.move_west(callback = callback),
        lambda callback: myla.move_west(callback = callback),
        lambda callback: myla.move_west(callback = callback),
        lambda callback: myla.face_east(callback = callback),
        lambda callback: engine.show_dialogue("Those crocodiles look pretty lazy, so they'll only be able to eat us if they're directly next to us!", callback = callback),
        lambda callback: engine.show_dialogue("I've put a script in your PyScripter called yell()", callback = callback),
        lambda callback: engine.clear_scripter(callback = callback),
        lambda callback: engine.insert_to_scripter("yell()", callback = callback),
        lambda callback: engine.show_dialogue("Run it to yell and distract a crocodile if one is about to eat me!", callback = callback),
        lambda callback: engine.show_dialogue("I'll follow your lead!", callback = callback),
        lambda callback: player_one.set_busy(False, callback =callback),
        lambda callback: myla.face_south(callback = callback)
        ]

        def myla_follow_player():
            global myla_follow
            myla_follow = True
            engine.register_input_callback(engine.INPUT_UP, lambda: follow(myla))
            engine.register_input_callback(engine.INPUT_DOWN, lambda: follow(myla))

        engine.run_callback_list_sequence(challenge2_dialogue, myla_follow_player)


def player_walked_on_challenge2b():
    global first_time_b;
    global myla_follow
    if first_time_b:
        first_time_b = False
        challenge2_dialogue = [
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: myla.face_east(callback = callback),
        lambda callback: engine.show_dialogue("Myla: You did it!!", callback = callback),
        lambda callback: player_one.face_west(callback = callback),
        lambda callback: myla.move_east(callback = callback),
        lambda callback: myla.move_east(callback = callback),
        lambda callback: myla.move_east(callback = callback),
        lambda callback: myla.move_east(callback = callback),
        lambda callback: myla.move_east(callback = callback),
        lambda callback: myla.move_east(callback = callback),
        lambda callback: myla.move_east(callback = callback),
        lambda callback: myla.face_east(callback = callback),
        lambda callback: engine.show_dialogue("My legs are getting tired, let me get into your bag!", callback = callback),
        lambda callback: myla.move_to((36,14), 0.0001, callback = callback),
        lambda callback: player_one.set_busy(False, callback =callback),
        ]
        myla_follow = False
        engine.run_callback_list_sequence(challenge2_dialogue)



    
challenge2a.player_walked_on = lambda player_object: player_walked_on_challenge2a()
challenge2b.player_walked_on = lambda player_object: player_walked_on_challenge2b()


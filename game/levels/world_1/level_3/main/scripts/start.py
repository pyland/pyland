#commence save-data set-up
world_name = "world_1"
level_name = "level_3"
map_name = "main"

engine.update_world_text("1")
engine.update_level_text("2")

player_data.load(engine.get_player_name())
player_data.set_map(world_name, level_name = level_name, map_name = map_name) #change the map and save that the map has changed
#end save-data set-up

need_croc_warning = True
first_time_a = True
first_time_b = True

def player_walked_on_myla_warn_crocs():
    player_data.save_checkpoint("myla_warn_crocs")
    global need_croc_warning
    if need_croc_warning:
        need_croc_warning = False
        engine.run_callback_list_sequence([
            lambda callback: player_one.set_busy(True, callback = callback),
            lambda callback: myla.wait(0.3, callback = callback),
            lambda callback: myla.stop_follow(callback = callback),
            lambda callback: myla.face_north(callback = callback),
            lambda callback: player_one.face_south(callback = callback),
            lambda callback: engine.show_dialogue("We have some lively crocs here, so I'm just gonna hop in your bag again...", callback = callback),
            lambda callback: player_one.face_north(callback = callback),
            lambda callback: myla.set_solidity(False, callback = callback),
            lambda callback: myla.move_north(callback = callback),
            lambda callback: myla.set_visible(False, callback = callback),
            lambda callback: engine.show_dialogue("Sweet, some chocolate.", callback = callback),
            lambda callback: engine.show_dialogue("Oh no, it's all melted in here, " + engine.get_player_name() + ", seriously, clear out your bag!", callback = callback),
            lambda callback: player_one.set_busy(False, callback =callback)
        ])

def player_walked_on_challenge2a():
    player_data.save_checkpoint("challenge2a")
    global first_time_a
    if first_time_a:
        first_time_a = False
        engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: player_one.face_west(callback = callback),
        lambda callback: engine.show_dialogue("Myla: Good job!", callback = callback),
        lambda callback: engine.show_dialogue("Those sleeping crocodiles are blocking our bag.", callback = callback),
        lambda callback: engine.show_dialogue("I'm puting a script in your PyScripter called yell().", callback = callback),
        lambda callback: engine.clear_scripter(callback = callback),
        lambda callback: engine.insert_to_scripter("yell()", callback = callback),
        lambda callback: engine.show_dialogue("Face the crocodiles and run the script in order to startle them and wake them up", callback = callback),
        lambda callback: engine.show_dialogue("Make sure you get out of the way before they get to you though!", callback = callback),
        lambda callback: engine.show_dialogue("Oh and the chocolate was delicious by the way.", callback = callback),
        lambda callback: player_one.set_busy(False, callback =callback),
        ])

def player_walked_on_challenge2b():
    player_data.save_checkpoint("challenge2b")
    global first_time_b
    if first_time_b:
        first_time_b = False
        challenge2_dialogue = [
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("Myla: You did it!!", callback = callback),
        lambda callback: engine.show_dialogue("Let me out of your bag now! It's getting a bit stuffy.", callback = callback),
        lambda callback: player_one.set_busy(False, callback =callback),
        lambda callback: player_one.move_west(callback = callback),
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: myla.set_solidity(False, callback = callback),
        lambda callback: myla.move_to(player_one.get_position(), callback = callback),
        lambda callback: myla.set_visible(True, callback = callback),
        lambda callback: myla.move_east(callback = callback),
        lambda callback: myla.face_west(callback = callback),
        lambda callback: player_one.face_east(callback = callback),
        lambda callback: myla.wait(0.3, callback = callback),
        lambda callback: myla.face_north(callback = callback),
        lambda callback: myla.wait(0.3, callback = callback),
        lambda callback: engine.show_dialogue("Take that you green stupid lungerheads!", callback = callback),
        lambda callback: engine.show_dialogue("Swinging monkeys! ... (TODO: Carl arrives)", callback = callback),
        lambda callback: myla.follow(player_one, callback = callback),
        lambda callback: player_one.set_busy(False, callback =callback),
        ]

        engine.run_callback_list_sequence(challenge2_dialogue)

#setting the player's starting position
if player_data.previous_exit_is(world_name, level_name = level_name, map_name = map_name, info = "myla_warn_crocs"):
    x, y = myla_warn_crocs.get_position()
    player_one.move_to((x, y),callback = lambda: myla.move_to((x, y-1), callback = player_walked_on_myla_warn_crocs))
elif player_data.previous_exit_is(world_name, level_name = level_name, map_name = map_name, info = "challenge2a"):
    need_croc_warning = False
    x, y = challenge2a.get_position()
    player_one.move_to((x, y), callback = player_walked_on_challenge2a)
elif player_data.previous_exit_is(world_name, level_name = level_name, map_name = map_name, info = "challenge2b"):
    need_croc_warning = False
    first_time_a = False
    x, y = challenge2b.get_position()
    player_one.move_to((x, y), callback = player_walked_on_challenge2b)
elif player_data.previous_exit_is("world_1"):
    player_one.move_north()
    myla_start_position = exit_level_start.get_position()
    myla.set_solidity(False, callback = lambda: myla.move_to(myla_start_position, callback = lambda: myla.follow(player_one)))
#end setting the player's starting position


def go_to_world(player_object):
    player_data.complete_level_and_save()
    player_data.save_and_exit("/world_1")

exit_level_start.player_walked_on = lambda player_object: player_data.save_and_exit("/world_1")
exit_level_end.player_walked_on = go_to_world #TODO: have this save that the level has been completed

player_one.focus()
engine.play_music("beach")

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

for i in range(len(b1_croc)):
    croc = b1_croc[i]
    croc.change_state("swim")

    if i % 2 == 0:
        croc.face_east()
    else:
        croc.face_west()
    croc.move_horizontal(player_one)


b2_croc = [croc_b2_1,
croc_b2_2,
croc_b2_3,
croc_b2_4,
croc_b2_5,
croc_b2_6,
croc_b2_7,
croc_b2_8,
croc_b2_9,
croc_b2_10
]

for croc in [croc_b2_1, croc_b2_2, croc_b2_3, croc_b2_7,croc_b2_8,croc_b2_9,croc_b2_10]:
    croc.face_east()
    croc.change_state("swim")

for croc in [croc_b2_4,croc_b2_5,croc_b2_6]:
    croc.face_west()
    croc.change_state("swim")

trigger_croc = [trigger_croc_0,
trigger_croc_1,
trigger_croc_2,
trigger_croc_3,
trigger_croc_4,
trigger_croc_5,
trigger_croc_6,
trigger_croc_7,
trigger_croc_8,
trigger_croc_9,
trigger_croc_10]


for croc in b1_croc:
    croc.go_to_checkpoint = lambda: engine.change_map("/world_1/level_3/mainb")
for croc in b2_croc:
    croc.go_to_checkpoint = lambda: engine.change_map("/world_1/level_3/mainb")


def player_walked_on_ti():
    x,y = player_one.get_position()
    for a in [-1, 1]:
        for obj in engine.get_objects_at((x+a,y)):
            if obj in b2_croc:
                return obj.lose(player_one)
        for obj in engine.get_objects_at((x,y+a)):
            if obj in b2_croc:
                return obj.lose(player_one)

for t in trigger_croc:
    t.player_walked_on = lambda player_object: player_walked_on_ti()

myla_warn_crocs.player_walked_on = lambda player_object: player_walked_on_myla_warn_crocs()
challenge2a.player_walked_on = lambda player_object: player_walked_on_challenge2a()
challenge2b.player_walked_on = lambda player_object: player_walked_on_challenge2b()



#commence save-data set-up
world_name = "world_1"
level_name = "level_4"
map_name = "main_b"

engine.update_world_text("1")
engine.update_level_text("4")

player_data.load(engine.get_player_name())
player_data.set_map(world_name, level_name = level_name, map_name = map_name) #change the map and save that the map has changed
#end save-data set-up

player_one.focus()

croc_0.face_east()
croc_1.face_east()
croc_2.face_east()

croc = [
croc_0,
croc_1,
croc_2,
croc_3,
croc_4,
croc_5
]

croc_4.oscillate = 1
croc_5.oscillate = 2

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
trigger_7,
trigger_8,
trigger_9,
trigger_10,
trigger_11,
trigger_12,
trigger_13,
trigger_14,
trigger_15,
trigger_16,
trigger_17,
trigger_18,
trigger_19,
trigger_20,
trigger_21,
trigger_22,
trigger_23,
trigger_24,
trigger_25,
trigger_26,
trigger_27,
trigger_28,
trigger_29]


def player_walked_on_ti():
    x,y = player_one.get_position()

    for obj in engine.get_objects_at((x+1,y)):
        if obj in croc:
            return obj.face_west(lambda: obj.lose(player_one))
    for obj in engine.get_objects_at((x-1,y)):
        if obj in croc:
            return obj.face_east(lambda: obj.lose(player_one))
    for obj in engine.get_objects_at((x,y+1)):
        if obj in croc:
            return obj.face_south(lambda: obj.lose(player_one))
    for obj in engine.get_objects_at((x,y-1)):
        if obj in croc:
            return obj.face_north(lambda: obj.lose(player_one))

for t in triggers:
    t.player_walked_on = lambda player_object: player_walked_on_ti()

engine.run_callback_list_sequence([
    lambda callback: player_one.face_east(callback = callback),
    lambda callback: player_one.set_busy(True, callback = callback),
    lambda callback: myla.face_west(callback = callback),
    lambda callback: engine.show_dialogue("Lots and lots and lots of crocodiles are out...", callback = callback),
    lambda callback: engine.show_dialogue("In your bag I go!", callback = callback),
    lambda callback: player_one.face_west(callback = callback),
    lambda callback: myla.set_solidity(False, callback = callback),
    lambda callback: myla.move_west(callback = callback),
    lambda callback: myla.set_visible(False, callback = callback),
    lambda callback: engine.show_dialogue("Sweet, you have a book I can read in here!", callback = callback),
    lambda callback: engine.show_dialogue("Oh it's \"Jungle Book\", far too unrealistic for me to enjoy.", callback = callback),
    lambda callback: player_one.set_busy(False, callback =callback)
    ])


first_time_end = True
def player_walked_on_end():
    global first_time_end;
    if first_time_end:
        first_time_end = False
        engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("Myla: You did it!", callback = callback),
        lambda callback: myla.move_to(player_one.get_position(), callback = callback),
        lambda callback: myla.set_visible(True, callback = callback),
        lambda callback: myla.move_south(callback = callback),
        lambda callback: myla.face_north(callback = callback),
        lambda callback: engine.show_dialogue("Myla: See you later alligator!", callback = callback),
        lambda callback: engine.show_dialogue("In a while crocodile!", callback = callback),
        lambda callback: player_one.set_busy(False, callback =callback),
        lambda callback: myla.follow(player_one, callback = callback),
        ])

def level_end(player_object):
    player_data.complete_level_and_save()
    player_data.save_and_exit("/world_1")

trigger_end.player_walked_on = lambda player_object: player_walked_on_end()
level_exit.player_walked_on = level_end

def prev_level(player_object):
    player_data.save_and_exit("/world_1/level_4/main_a")

exit_level_start1.player_walked_on = prev_level
exit_level_start2.player_walked_on = prev_level


#commence save-data set-up
world_name = "world1"
level_name = "level2"
map_name = "main_b"

engine.update_world_text("1")
engine.update_level_text("2")

player_data.load(engine.get_player_name())
player_data.set_map(world_name, level_name = level_name, map_name = map_name) #change the map and save that the map has changed
#end save-data set-up

player_one.focus()

croc_1.face_east()
croc_2.face_east()

croc = [
croc_1,
croc_2,
croc_4,
croc_5
]

croc[0].face_east()
croc[1].face_east()
croc[2].face_south()
croc[3].face_west()

engine.run_callback_list_sequence([
    lambda callback: player_one.face_east(callback = callback),
    lambda callback: player_one.set_busy(True, callback = callback),
    lambda callback: myla.face_west(callback = callback),
    lambda callback: engine.show_dialogue("Myla: The crocodiles are blocking the path to get acoss the bridge!", callback = callback),
    lambda callback: engine.show_dialogue("Try and yell at them so we can get across it, but don't get too close to them!", callback = callback),
    lambda callback: myla.follow(player_one, callback = callback),
    lambda callback: player_one.face_north(callback =callback),
    lambda callback: player_one.set_busy(False, callback =callback)
    ])


yell_again = [
    lambda callback: engine.show_dialogue("Use this script by facing the crocodiles and clicking 'Run'", callback = callback),
    lambda callback: engine.clear_scripter(callback = callback),
    lambda callback: engine.insert_to_scripter("yell()", callback = callback),
    lambda callback: engine.show_dialogue("Maybe you can use the blue crocodile to trap the white crocodile before it gets back to the bridge!", callback = callback),
    lambda callback: player_one.set_busy(False, callback = callback)
]

not_yell_again = [
    lambda callback: engine.show_dialogue("Ok, maybe you can use the blue crocodile to trap the white crocodile before it gets back to the bridge!", callback = callback),
    lambda callback: player_one.set_busy(False, callback = callback)
]

myla_sequence = [
    lambda callback: player_one.set_busy(True, callback = callback),
    lambda callback: myla.turn_to_face(player_one, callback = callback),

    lambda callback: engine.show_dialogue_with_options(
        "Would you like the yell script again?",
        {
            "Yes": lambda: engine.run_callback_list_sequence(yell_again),
            "No" : lambda: engine.run_callback_list_sequence(not_yell_again)
        }
    )

]

myla.player_action = lambda player_object: engine.run_callback_list_sequence(myla_sequence)

first_time_end = True
def player_walked_on_end():
    global first_time_end;
    if first_time_end:
        first_time_end = False
        engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: myla.stop_follow(callback = callback),
        lambda callback: engine.show_dialogue("Myla: You did it!", callback = callback),
        lambda callback: myla.face_south(callback = callback),
        lambda callback: engine.show_dialogue("See you later alligator!", callback = callback),
        lambda callback: engine.show_dialogue("In a while crocodile!", callback = callback),
        lambda callback: player_one.set_busy(False, callback =callback),
        lambda callback: myla.follow(player_one, callback = callback),
        ])

def level_end(player_object):
    player_data.complete_level_and_save()
    player_data.save_and_exit("/world1")

trigger_end.player_walked_on = lambda player_object: player_walked_on_end()
level_exit.player_walked_on = level_end

def prev_level(player_object):
    player_data.save_and_exit("/world1/level2/main_a")

exit_level_start1.player_walked_on = prev_level
exit_level_start2.player_walked_on = prev_level


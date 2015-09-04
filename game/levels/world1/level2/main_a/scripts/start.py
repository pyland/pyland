#commence save-data set-up
world_name = "world_1"
level_name = "level_2"
map_name = "main_a"

engine.update_world_text("1")
engine.update_level_text("2")
engine.play_music("myla")

player_data.load(engine.get_player_name())
player_data.set_map(world_name, level_name = level_name, map_name = map_name) #change the map and save that the map has changed
#end save-data set-up

came_from_main_b = False

if player_data.previous_exit_is(world_name, level_name = level_name, map_name = "main_b"):
    x, y = end_level.get_position()
    came_from_main_b = True
    player_one.move_to((x, y-1), callback = lambda: myla.move_to((x-1, y-1), callback = lambda: myla.follow(player_one)))
player_one.focus()

croc_0.face_east()
croc_1.face_east()
croc_2.face_east()
croc_3.face_east()

croc_0.killable = [player_one, myla]
croc_1.killable = [player_one, myla]
croc_2.killable = [player_one, myla]
croc_3.killable = [player_one, myla]
croc_0.still_check_kill()
croc_1.still_check_kill()
croc_2.still_check_kill()
croc_3.still_check_kill()

if not came_from_main_b:
    engine.run_callback_list_sequence([
        lambda callback: player_one.face_east(callback = callback),
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: myla.face_west(callback = callback),
        lambda callback: engine.show_dialogue("Myla: Looks like we've got some sleeping crocodiles here.", callback = callback),
        lambda callback: engine.show_dialogue("The scent of someone walking past wakes them up. We should try to stop them blocking paths to make Pyland a safer place to live.", callback = callback),
        lambda callback: engine.show_dialogue("Here's a script for your PyScripter that wakes them up.", callback = callback),
        lambda callback: engine.clear_scripter(callback = callback),
        lambda callback: engine.insert_to_scripter("yell()", callback = callback),
            lambda callback: engine.show_dialogue("Click 'Run' in the PyScripter below when you're facing a crocodile to wake them up.", callback = callback),
        lambda callback: engine.show_dialogue("Try yelling at these ones, you should be safe with the rocks.", callback =callback),
        lambda callback: engine.show_dialogue("And don't hesistate to talk to me again if you accidently lose the script!", callback = callback),

        lambda callback: myla.follow(player_one, callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback)
        ])

yell_again = [
    lambda callback: engine.show_dialogue("Use this script by facing the crocodiles and clicking 'Run'.", callback = callback),
    lambda callback: engine.clear_scripter(callback = callback),
    lambda callback: engine.insert_to_scripter("yell()", callback = callback),
    lambda callback: player_one.set_busy(False, callback = callback)
]

not_yell_again = [
    lambda callback: engine.show_dialogue("Ok, make sure you face the crocodiles you're trying to yell at!", callback = callback),
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



def go_next(player_object):
    player_data.save_and_exit("/world1/level2/main_b")

end_level.player_walked_on = go_next
end_level_2.player_walked_on = go_next

def leave_beg(player_object):
    player_data.save_and_exit("/world1")

exit_level_start.player_walked_on = leave_beg


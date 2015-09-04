#TODO: If you give the correct script, the players dont' ask for help and let you modify the script without giving them the one with errors each time

#commence save-data set-up
world_name = "world1"
level_name = "level4"
map_name = "main"

player_data.load(engine.get_player_name())
player_data.set_map(world_name, level_name = level_name, map_name = map_name)

engine.play_music("world_1_myla")

engine.update_world_text("1")
engine.update_level_text("3")

finish_level = [
    lambda callback: engine.set_py_tabs(2, callback = callback),
    lambda callback: engine.show_dialogue("You have unlocked another PyScripter Tab!", callback = callback),
    lambda callback: engine.show_dialogue("You can now store two scripts in you PyScripter. They can be run at any time using '1' or '2' on your keyboard", callback = callback),
    lambda callback: player_data.save_and_exit("/world1"),
]

start.player_walked_on = lambda player_object: player_data.save_and_exit("/world1")
end.player_walked_on = lambda player_object: engine.run_callback_list_sequence(finish_level)

engine.set_py_tabs(1)

player_one.focus()
player_one.face_south()
myla.follow(player_one)

horizontal_crocodiles = [croc_0, croc_1]
all_crocodiles = [croc_0, croc_1, croc_2, croc_3, croc_4, croc_5, croc_6, croc_7]


for croc in horizontal_crocodiles:
    croc.face_east()
    croc.change_state("swim")
    croc.move_horizontal()
    croc.killable = [player_one, myla]
    croc.still_check_kill()

for croc in all_crocodiles:
    croc.killable = [player_one, myla]
    croc.still_check_kill()



def write_script(challenger, dialogue):
    challenger.turn_to_face(player_one)
    engine.run_callback_list_sequence([
        lambda callback: challenger.set_busy(False, callback = callback),
        lambda callback: engine.show_external_script(
            confirm_callback = lambda: engine.run_callback_list_sequence([
                lambda callback: player_one.set_busy(False, callback = callback),
                lambda callback: challenger.run_script(script_to_run = 10)]),
            cancel_callback = lambda: player_one.set_busy(False),
            external_dialogue = dialogue,
            character_object = challenger
        )
    ])

def fix_script(challenger, dialogue, script):
    challenger.turn_to_face(player_one)
    challenger.set_script(script)
    challenger.stop_turning()
    engine.run_callback_list_sequence([
        lambda callback: challenger.set_busy(False, callback = callback),
        lambda callback: engine.show_external_script(
            confirm_callback = lambda: engine.run_callback_list_sequence([
                lambda callback: player_one.set_busy(False, callback = callback),
                lambda callback: npc.start_turning(4.0,10, callback = callback),
                lambda callback: challenger.run_script(script_to_run = 10)]),
            cancel_callback = lambda: engine.run_callback_list_sequence([
                lambda callback: player_one.set_busy(False, callback = callback),
                lambda callback: npc.start_turning(4.0,10, callback = callback)]),
            external_dialogue = dialogue,
            character_object = challenger
        )
    ])

npcs = [ben,ruby,sid,cindy,joya,ying]

for npc in npcs:
    npc.start_turning(4.0,10)

npcs[0].player_action = lambda player_object: fix_script(npcs[0],"Can you fix my script so I get out of the way of the path?","movewest")
npcs[1].player_action = lambda player_object: fix_script(npcs[1],"I wish I wasn't blocking the path!","west")
npcs[2].player_action = lambda player_object: write_script(npcs[2],"Can you write me script so I'm not blocking the path?")
npcs[3].player_action = lambda player_object: write_script(npcs[3],"I'd love if I could have my own script!")
npcs[4].player_action = lambda player_object: fix_script(npcs[4],"Could you fix my yell command so I can stop that crocodile blocking the path?","face_east()\nyel()")
npcs[5].player_action = lambda player_object: fix_script(npcs[5],"If only this script would yell at both crocodiles!","move_west()\nface_north()\nyell()\nmove_west()\nmove_west()\nface_north()")

myla_sequence = [
    lambda callback: player_one.set_busy(True, callback = callback),
    lambda callback: myla.turn_to_face(player_one, callback = callback),
    lambda callback: engine.show_dialogue("Lets see if we can help all these people and get to the end of the path.", callback = callback),
    lambda callback: player_one.set_busy(False, callback = callback),
]

myla.player_action = lambda player_object: engine.run_callback_list_sequence(myla_sequence)

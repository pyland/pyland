#TODO: If you give the correct script, the players dont' ask for help and let you modify the script without giving them the one with errors each time
#TODO: Only trigger run_info events if the move_north() script was run, not just if it is in the scripter

#commence save-data set-up
world_name = "world1"
level_name = "level3"
map_name = "main"

player_data.load(engine.get_player_name())
player_data.set_map(world_name, level_name = level_name, map_name = map_name)

engine.play_music("world_1_jungle")

engine.update_world_text("1")
engine.update_level_text("3")

start.player_walked_on = lambda player_object: player_data.save_and_exit("/world1")
end.player_walked_on = lambda player_object: player_data.save_and_exit("/world1")


engine.set_py_tabs(1)

player_one.focus()
player_one.face_south()

horizontal_crocodiles = [croc_0]
all_crocodiles = [croc_0, croc_1, croc_2, croc_3]

for croc in horizontal_crocodiles:
    croc.face_east()
    croc.change_state("swim")
    croc.move_horizontal()
    croc.killable = [player_one]
    croc.still_check_kill()

for croc in all_crocodiles:
    croc.killable = [player_one]
    croc.still_check_kill()


engine.run_callback_list_sequence([
        lambda callback: player_one.face_east(callback = callback),
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: myla.turn_to_face(player_one,callback = callback),
        lambda callback: player_one.turn_to_face(myla,callback = callback),
        lambda callback: engine.show_dialogue("Myla: All these people seem to be stuck in the paths!", callback = callback),
        lambda callback: engine.show_dialogue("Lets try and help them out by fixing their scripts!", callback = callback),
        lambda callback: engine.clear_scripter(callback = callback),
        lambda callback: engine.insert_to_scripter("move_north()", callback = callback),
        lambda callback: engine.show_dialogue("You can write scripts to move like this one:", callback = callback),
        lambda callback: engine.show_dialogue("Give it a run!", callback = callback),
        lambda callback: myla.follow(player_one, callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback)
])

run_sequence = [
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: myla.turn_to_face(player_one,callback = callback),
        lambda callback: engine.show_dialogue("Lets go and talk to the players and fix their scripts.", callback = callback),
        lambda callback: engine.show_dialogue("You need to make sure commands are spelt exactly like move_north() .", callback = callback),
        lambda callback: myla.follow(player_one, callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback)
]

done_run = False

def run_info():
    global done_run
    if (not done_run) and (engine.get_script() == "move_north()") :
        engine.run_callback_list_sequence(run_sequence)
        done_run = True

trigger_run.player_walked_on = lambda player_object: run_info()

yell_sequence = [
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: myla.turn_to_face(player_one,callback = callback),
        lambda callback: player_one.turn_to_face(myla,callback = callback),
        lambda callback: engine.show_dialogue("More sleeping crocodiles! Here's the yell script again, I think you'll need it!", callback = callback),
        lambda callback: engine.clear_scripter(callback = callback),
        lambda callback: engine.insert_to_scripter("yell()", callback = callback),
        lambda callback: engine.show_dialogue("If you can't remember the name of a command, check out the PyGuide. It's in your bag - that green bag in the top right.", callback = callback),
        lambda callback: myla.follow(player_one, callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback)
]

given_yell = False

def yell_info():
    global given_yell
    if not given_yell:
        engine.run_callback_list_sequence(yell_sequence)
        given_yell = True

trigger_yell.player_walked_on = lambda player_object: yell_info()

def write_script(challenger, dialogue, script):
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

npcs = [louis,anya,harriet,kushan]

for npc in npcs:
    npc.face_east()
    npc.start_turning(3.0,20)

louis.player_action = lambda player_object: write_script(louis,"I'm stuck here and blocking the path! Please fix my script and then click 'Give Script' in the PyScripter below.","mve_north()")
anya.player_action = lambda player_object: write_script(anya,"Please fix my script and then click 'Give Script'.","move_west()\nmve_west()")
harriet.player_action = lambda player_object: write_script(harriet,"Please make my script move me so I'm not blocking the path!","mve_west()\nmove_noth()\nmove_nort()\nmove_orth()\nmve_west()")
kushan.player_action = lambda player_object: write_script(kushan,"I'm not sure why script isn't working, can you fix it?","movewest()")



yell_again = [
    lambda callback: engine.show_dialogue("Here you go!", callback = callback),
    lambda callback: engine.clear_scripter(callback = callback),
    lambda callback: engine.insert_to_scripter("move_north()", callback = callback),
    lambda callback: player_one.set_busy(False, callback = callback)
]

not_yell_again = [
    lambda callback: player_one.set_busy(False, callback = callback)
]

myla_sequence = [
    lambda callback: player_one.set_busy(True, callback = callback),
    lambda callback: myla.turn_to_face(player_one, callback = callback),

    lambda callback: engine.show_dialogue_with_options(
        "Would you like the movement script again?",
        {
            "Yes": lambda: engine.run_callback_list_sequence(yell_again),
            "No" : lambda: engine.run_callback_list_sequence(not_yell_again)
        }
    )

]

myla.player_action = lambda player_object: engine.run_callback_list_sequence(myla_sequence)

#commence save-data set-up

player_one.focus()
player_one.face_east()

kanade.face_south()



def challenger_action_move(player_object, challenger, dialogue_list = None, end_list = None):
    challenger.turn_to_face(player_object)
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.run_callback_list_sequence(dialogue_list, callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback),
        lambda callback: engine.run_callback_list_sequence(end_list, callback = callback),


    ])

def write_script(challenger):
    engine.run_callback_list_sequence([
        lambda callback: challenger.set_busy(False, callback = callback),
        lambda callback: engine.show_external_script(
            confirm_callback = lambda: engine.run_callback_list_sequence([
                lambda callback: player_one.set_busy(False, callback = callback),
                lambda callback: challenger.run_script(script_to_run = 10)]),
            cancel_callback = lambda: player_one.set_busy(False),
            external_dialogue = "Aren't PyScripters cool?",
            character_object = challenger
        )
    ])

kanade_dialogue = ([
    lambda callback: engine.show_dialogue("Kanade: My eyes aren't what they used to be...", callback = callback),
    lambda callback: engine.show_dialogue("Luckily my PyRunner can tell me if I'm about to walk into a wall", callback = callback),
    lambda callback: engine.clear_scripter(callback = callback),
    lambda callback: engine.insert_to_scripter("print(can_move())", callback = callback),
    lambda callback: engine.show_dialogue("Try it out! I've put my script into your PyScripter; try running it while facing a rock!", callback = callback)
    ])

kanade_end = ([
    lambda callback: kanade.move_east()
    ])

kanade.player_action = lambda player_object: challenger_action_move(player_object, kanade, kanade_dialogue, kanade_end)

erick_dialogue = ([
    lambda callback: engine.show_dialogue("Erick: I LIKE TO MOVE IT, MOVE IT", callback = callback),
    lambda callback: engine.show_dialogue("I LIKE TO MOVE IT, MOVE IT", callback = callback),
    lambda callback: engine.show_dialogue("YOU LIKE TO...MOVE IT!", callback = callback),
    lambda callback: engine.insert_to_scripter("\nmove()", callback = callback),
    lambda callback: engine.show_dialogue("Bro, check this script out!", callback = callback),
    lambda callback: engine.show_dialogue("It moves you in whatever direction you're facing! It does the job of four other scripts!", callback = callback),
    lambda callback: engine.show_dialogue("Watch me move!", callback = callback),

    ])

erick_end = ([
    lambda callback: erick.move_north(callback = callback),
    lambda callback: erick.move_north()
    ])

erick.player_action = lambda player_object: challenger_action_move(player_object, erick, erick_dialogue, erick_end)

derek_dialogue = ([
    lambda callback: engine.show_dialogue("Derek: I love turning right!", callback = callback),
    lambda callback: engine.show_dialogue("Sadly, I'm not an ambiturner...I can't turn left", callback = callback),
    lambda callback: engine.insert_to_scripter("\nturn_right()", callback = callback),
    lambda callback: engine.show_dialogue("Now you can turn right too!", callback = callback),
    lambda callback: engine.show_dialogue("Maybe you can figure out how to turn left", callback = callback)
    ])

derek_end = ([
    lambda callback: derek.move_east(callback = callback),
    lambda callback: derek.move_east()
    ])


derek.player_action = lambda player_object: challenger_action_move(player_object, derek, derek_dialogue, derek_end)

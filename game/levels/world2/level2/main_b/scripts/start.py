#commence save-data set-up

player_one.focus()
player_one.face_east()

yuri.face_west()

def challenger_action(player_object, challenger, dialogue_list = None, end_list = None):
    challenger.turn_to_face(player_object)
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.run_callback_list_sequence(dialogue_list, callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback),
        lambda callback: engine.run_callback_list_sequence(end_list, callback = callback),


    ])

yuri_dialogue = ([
    lambda callback: engine.show_dialogue("Yuri: Its easy to get lost in this maze", callback = callback),
    lambda callback: engine.show_dialogue("Maybe this script will prove to be useful", callback = callback),
    lambda callback: engine.clear_scripter(callback = callback),
    lambda callback: engine.insert_to_scripter("while True:\n  if not can_move():\n     turn_left()\n  move()", callback = callback),
    lambda callback: engine.show_dialogue("The second line may be new to you, but its called an if statement", callback = callback),
    lambda callback: engine.show_dialogue("It checks the condition block, which in this case is not can_move() and if it is true runs the if block, which is turn_left()", callback = callback),
    lambda callback: engine.show_dialogue("This script checks if you can move forward. If you can't it turns you left", callback = callback),
    ])

yuri_end = ([
    lambda callback: yuri.move_east(callback = callback),
    lambda callback: yuri.move_north()
    ])

pia_dialogue = ([
    lambda callback: engine.show_dialogue("Help! I'm lost! The code that Yuri gave me got me this far, but it doesn't work for the rest of the maze", callback = callback),
    lambda callback: engine.show_dialogue("Please help me!", callback = callback),
    lambda callback: write_script(pia),
    ])

def write_script(challenger):
    engine.run_callback_list_sequence([
        lambda callback: challenger.set_busy(False, callback = callback),
        lambda callback: engine.show_external_script(
            confirm_callback = lambda: engine.run_callback_list_sequence([
                lambda callback: player_one.set_busy(False, callback = callback),
                lambda callback: challenger.run_script(script_to_run = 10)]),
            cancel_callback = lambda: player_one.set_busy(False),
            external_dialogue = "Help!",
            character_object = challenger
        )
    ])

pia.set_script("while True:\n  if not can_move():\n     turn_left()\n  move()")
yuri.player_action = lambda player_object: challenger_action(player_object, yuri, yuri_dialogue, yuri_end)
pia.player_action = lambda player_object: challenger_action(player_object, pia, pia_dialogue)


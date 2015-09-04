#commence save-data set-up

player_one.focus()
player_one.face_east()

challengers = [mita, arnav, advaith]

challengers[0].face_west()
challengers[0].set_cuts_left(7)
challengers[1].face_south()
challengers[1].set_cuts_left(8)
challengers[2].face_south()
challengers[2].set_cuts_left(9)



def challenger_action(player_object, challenger):
    challenger.turn_to_face(player_object)
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: challenger.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("Take a look at this cool device I found!.", callback = callback),
        lambda callback: write_script(challenger)
    ])


def write_script(challenger):
    engine.run_callback_list_sequence([
        lambda callback: challenger.set_busy(False, callback = callback),
        lambda callback: engine.show_external_script(
            confirm_callback = lambda: engine.run_callback_list_sequence([
                lambda callback: player_one.set_busy(False, callback = callback),
                lambda callback: challenger.run_script(script_to_run = 10)]),
            cancel_callback = lambda: player_one.set_busy(False),
            external_dialogue = "Isn't this neat?",
            character_object = challenger
        )
    ])

challengers[0].player_action = lambda player_object: challenger_action(player_object, challengers[0])
challengers[1].player_action = lambda player_object: challenger_action(player_object, challengers[1])
challengers[2].player_action = lambda player_object: challenger_action(player_object, challengers[2])
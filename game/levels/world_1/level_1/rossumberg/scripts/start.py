player_one.focus()

engine.play_music("calm")
engine.set_ui_colours((200,255,200),(215,255,215)) #TODO: save these colours in the config.

dialogue_sequence = [
    lambda callback: player_one.set_busy(True, callback = callback),
    lambda callback: player_one.face_south(callback = callback),
    lambda callback: engine.show_dialogue("afasfasfsfasfaf!!!!", callback = callback),
    lambda callback: heidi.face_west(callback = callback),
    lambda callback: engine.show_dialogue("%player_name%!!!!", callback = callback),
    lambda callback: heidi.move_west(callback = callback),
    lambda callback: heidi.move_west(callback = callback),
    lambda callback: heidi.move_west(callback = callback),
    lambda callback: heidi.move_west(callback = callback),
    lambda callback: heidi.move_north(callback = callback),
    lambda callback: heidi.move_west(callback = callback),
    lambda callback: heidi.move_west(callback = callback),
    lambda callback: player_one.face_east(callback = callback),
    lambda callback: engine.show_dialogue("Today is the big day! You did remember right?", callback = callback),
    #TODO: change the level to the intro level
    lambda callback: heidi.move_east(callback = callback),
    lambda callback: heidi.move_east(callback = callback),
    lambda callback: heidi.move_east(callback = callback),
    lambda callback: engine.show_dialogue("TESTSTSSTSTSTSTSTSTT", callback = callback),
    lambda callback: heidi.move_east(callback = callback),
    lambda callback: heidi.move_east(callback = callback),
    lambda callback: player_one.set_busy(False, callback = callback)
]
"""
engine.run_callback_list_sequence(
    dialogue_sequence,
    lambda: engine.show_dialogue_with_options(
        "Broo",
        {
            "south": lambda : player_one.move_south(),
            "printj" : lambda : engine.print_terminal("j")
        }
    )
) """
"""
engine.run_callback_list_sequence(
    dialogue_sequence
)
"""
engine.run_callback_list_sequence(
    dialogue_sequence,
    heidi.move_west
)

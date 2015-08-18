player_one.focus()

engine.play_music("calm")
engine.set_ui_colours((200,255,200),(215,255,215)) #TODO: save these colours in the config.

dialogue_sequence = [
    lambda callback: player_one.set_busy(True, callback = callback),
    lambda callback: player_one.face_south(callback = callback),
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
]

engine.run_callback_list_sequence(
    dialogue_sequence,
    lambda: engine.show_dialogue_with_options(
        "Today is the big day! You did remember right?",
        {
            "Yes": if_remembered_big_day,
            "No": lambda : player_one.move_north(lambda: player_one.set_busy(False, callback = callback)),
        }
    )
)

def if_remembered_big_day():
    sequence = [
        lambda callback: engine.show_dialogue("I thought you would!", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback),
    ]
    engine.run_callback_list_sequence(sequence)

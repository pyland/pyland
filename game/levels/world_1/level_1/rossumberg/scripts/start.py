player_one.focus()

engine.play_music("calm")
engine.set_ui_colours((200,255,200),(215,255,215)) #TODO: save these colours in the config.

dialogue_sequence = [
    lambda callback: player_one.set_busy(True, callback = callback),
    lambda callback: heidi.face_west(callback = callback),
    lambda callback: engine.show_dialogue("Hey, %player_name%!!!!", callback = callback),
    heidi.move_west,
    heidi.move_west,
    heidi.move_west,
    heidi.move_west,
    heidi.move_north,
    heidi.move_west,
    heidi.move_west,
    lambda callback: engine.show_dialogue("Use your WASD keys to move to the end of the path!", callback = callback),
    #TODO: change the level to the intro level
    lambda callback: player_one.set_busy(False, callback = callback)
]


end_dialogue = [
    lambda callback: player_one.set_busy(True, callback = callback),
    lambda callback: engine.show_dialogue("Congratulations! You've reached the end!", callback = callback),
    lambda callback: engine.show_dialogue("Here is a funny joke! \n What do you call an alligator in a vest?", callback = callback),
    #TODO: change the level to the intro level
    lambda callback: engine.show_dialogue("An investigator!", callback = callback),
    lambda callback: player_one.set_busy(False, callback = callback)
]

player_one.face_south()
engine.run_callback_list_sequence(dialogue_sequence)

end.dialogue = end_dialogue


player_one.focus()

engine.play_music("eerie")
engine.set_ui_colours((200,255,200),(215,255,215)) #TODO: save these colours in the config.

#The first time a player walks onto trigger_one
boulder_drop_sequence_first = [
    lambda callback: player_one.set_busy(True, callback = callback),
    lambda callback: boulder_one.set_sprite("", callback = callback),
    lambda callback: boulder_one.set_solidity(True, callback = callback),
    lambda callback: engine.show_dialogue("Oh no!", callback = callback),
    lambda callback: myla.move_east(callback = callback),
    lambda callback: myla.move_east(callback = callback),
    lambda callback: myla.move_east(callback = callback),
    lambda callback: myla.move_east(callback = callback),
    lambda callback: myla.move_east(callback = callback),
    lambda callback: myla.move_east(callback = callback),
    lambda callback: myla.move_east(callback = callback),
    lambda callback: myla.move_east(callback = callback),
    lambda callback: engine.show_dialogue("Guess you've to do this level by yourself!", callback = callback),
    lambda callback: player_one.set_busy(False, callback = callback),
]

#The first time a player walks onto trigger_two
boulder_drop_sequence_second = [
    lambda callback: player_one.set_busy(True, callback = callback),
    lambda callback: engine.show_dialogue("Guess you've to do this level by yourself!", callback = callback),
    lambda callback: player_one.set_busy(False, callback = callback),
]

def boulder_drop():
    if trigger_one.num_walk == 0 :
        engine.run_callback_list_sequence(boulder_drop_sequence_first)
        trigger_one.num_walk += 1;
    else:
        engine.run_callback_list_sequence(boulder_drop_sequence_second)

trigger_one.player_walked_on = lambda player_one : boulder_drop()

#Now the boulder is in place


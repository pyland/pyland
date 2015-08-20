player.focus()
engine.update_player_name(engine.get_player_name(), player.get_focus_button_id())

engine.play_music("eery")

engine.clear_scripter()

player.face_east()
myla.face_east()

player.set_busy(True)

dialogue_sequence = [
    lambda callback: engine.show_dialogue("How odd. It's strangely quiet here. This is usually a lively snake town.", callback = callback),
    lambda callback: myla.face_north(callback = callback),
    lambda callback: engine.show_dialogue("We should see if the minister is the town hall.", callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
    lambda callback: myla.wait(1.0, callback = callback),
    lambda callback: myla.move_east(callback = callback),
    lambda callback: myla.move_east(callback = callback),
    lambda callback: myla.move_east(callback = callback),
    lambda callback: myla.move_east(callback = callback),
    lambda callback: myla.move_east(callback = callback),
    lambda callback: myla.move_east(callback = callback),
    lambda callback: myla.turn_to_face(player, callback = callback),
    lambda callback: engine.show_dialogue("Come on then, lets go in!", callback = callback),
    lambda callback: myla.wait(1.0, callback = callback),
    lambda callback: myla.start_turning(2.5, 6, callback = callback),
    #lambda callback: camera.wait(0.2, callback = callback),
    #lambda callback: engine.show_dialogue(engine.get_dialogue("intro_im_monty_the_snake"), callback = callback),
    #lambda callback: engine.show_dialogue(engine.get_dialogue("intro_monty_doesnt_know_name"), callback = callback),
    #lambda callback: engine.show_dialogue(engine.get_dialogue("intro_monty_text_editor"), callback = callback),
    #lambda callback: engine.show_dialogue(engine.get_dialogue("intro_big_white_box"), callback = callback),
    #lambda callback: get_player_name(callback = callback),
    #lambda callback: engine.show_dialogue(engine.get_dialogue("intro_confirm_player_name", {"player_name": player_name}), callback = callback), #TODO, give option for dictionary of callback response
    #lambda callback: engine.show_dialogue(engine.get_dialogue("intro_wrote_first_program", {"player_name": player_name}), callback = callback),
    #lambda callback: engine.show_dialogue(engine.get_dialogue("intro_console_output"), callback = callback),
    #lambda callback: engine.show_dialogue(engine.get_dialogue("intro_go_enjoy_pyland", {"player_name": player_name}), callback = callback)
]

engine.run_callback_list_sequence(dialogue_sequence)

def go_to_hall(player_object):
    engine.change_map("/world_1/level_7/town_hall")

hall_entrance.player_walked_on = go_to_hall


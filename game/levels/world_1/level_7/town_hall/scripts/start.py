from random import randint

player.focus()
engine.update_player_name(engine.get_player_name(), player.get_focus_button_id())

engine.play_music("eery")
engine.update_level("7");

player.face_east()
player.set_busy(True)
myla.face_east()

leader.face_west()

snake1.face_east()
snake2.face_east()
snake3.face_east()
snake4.face_east()
snake5.face_east()
snake6.face_east()
snake7.face_east()

snake1.change_state("orange")
snake2.change_state("green")
snake3.change_state("red")
snake4.change_state("green")
snake5.change_state("red")
snake6.change_state("yellow")
snake7.change_state("orange")
leader.change_state("blue")


def pace(cur_object, callback = lambda: None):
    cur_object.move_north(lambda: cur_object.wait(1.0, lambda: cur_object.move_south(lambda: cur_object.move_south(lambda: cur_object.wait(1.0, lambda: cur_object.move_north(lambda: pace(cur_object)))))))
    callback()


dialogue_sequence = [
    lambda callback: engine.show_dialogue("Minister: Attention my friends. I am sorry to have to say this again.", callback = callback),
    lambda callback: engine.show_dialogue("The catchers are back.", callback = callback),
    lambda callback: snake1.start_turning(time = 0.1, frequency = 8, callback = callback),
    lambda callback: snake2.start_turning(time = 0.1, frequency = 8, callback = callback),
    lambda callback: snake3.start_turning(time = 0.1, frequency = 8, callback = callback),
    lambda callback: snake4.start_turning(time = 0.1, frequency = 8, callback = callback),
    lambda callback: snake5.start_turning(time = 0.1, frequency = 8, callback = callback),
    lambda callback: snake6.start_turning(time = 0.1, frequency = 8, callback = callback),
    lambda callback: snake7.start_turning(time = 0.1, frequency = 8, callback = callback),
    lambda callback: leader.move_north(callback = callback),
    lambda callback: leader.move_west(callback = callback),
    lambda callback: leader.move_west(callback = callback),
    lambda callback: leader.move_south(callback = callback),
    lambda callback: leader.face_west(callback = callback),
    lambda callback: engine.show_dialogue("Please stay calm. I can handle this.", callback = callback),
    lambda callback: engine.show_dialogue("But we must prepare to escape to the desert, in case they do not leave.", callback = callback),
    lambda callback: myla.move_east(callback = callback),
    lambda callback: engine.show_dialogue("Myla: Lets help them out, talk to as many of them as you can.", callback = callback),
    lambda callback: pace(leader, callback = callback),

    lambda callback: player.set_busy(False, callback = callback),
    lambda callback: myla.follow(player, callback = callback),
   # lambda callback: engine.enable_py_scripter(callback = callback)
]

player1_sequence = [
    lambda callback: player.set_busy(True, callback = callback),
    lambda callback: snake1.stop_turning(callback = callback),
    lambda callback: snake1.turn_to_face(player, callback = callback),
    lambda callback: engine.show_dialogue("I'm scared. I thought the catchers were just a myth.", callback = callback),
    lambda callback: engine.show_dialogue("My PyRunner script is broken. Can you fix it so I move east and get to the desert?", callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]

try_to_leave_sequence = [
    lambda callback: player.set_busy(True, callback = callback),
    lambda callback: engine.show_dialogue("Myla: Don't leave "+engine.get_player_name()+", they need our help!", callback = callback),
    lambda callback: myla.stop_follow(callback = callback),
    lambda callback: myla.move_east(callback = callback),
    lambda callback: player.move_east(callback = callback)
    #lambda callback: player.set_busy(False, callback = callback)
    #lambda callback: myla.follow(player, callback = callback)
]

snake1.player_action = lambda player_object: engine.run_callback_list_sequence(player1_sequence) #nake1_player_action

engine.run_callback_list_sequence(dialogue_sequence)

leave_hall.player_walked_on = lambda player_object: engine.run_callback_list_sequence(try_to_leave_sequence)

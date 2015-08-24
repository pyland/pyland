from random import randint

player.focus()
engine.update_player_name(engine.get_player_name(), player.get_focus_button_id())

engine.play_music("eery")
engine.update_level("7")
engine.set_py_tabs(9)

player.face_east()
player.set_busy(True)
myla.face_east()

leader.face_west()

snakes = [snake1,snake2,snake3,snake4,snake5,snake6,snake7]
snakes_initial_pos = []
snakes_final_pos = []

for snake in snakes:
    snakes_initial_pos.append((0,0))
    snakes_final_pos.append((0,0))

snake1.face_east()
snake2.face_east()
snake3.face_east()
snake4.face_east()
snake5.face_east()
snake6.face_east()
snake7.face_east()

snakes[0].change_state("orange")
snakes[1].change_state("green")
snakes[2].change_state("red")
snakes[3].change_state("green")
snakes[4].change_state("red")
snakes[5].change_state("yellow")
snakes[6].change_state("orange")

leader.change_state("blue")

#Store whether you have correctly helped each player yet
snake_helped = []
for x in range(7):
    snake_helped.append(0)

def update_snake_stage(snake = 0, stage = 1, callback = lambda: None):
    snake_helped[snake] = stage
    callback()

def update_snake_final_position(snake_index):
    snakes_final_pos[snake_index] = snakes[snake_index].get_position()

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
    #lambda callback: engine.show_dialogue("My PyRunner script is broken. Can you fix it so I move east and get to the desert?", callback = callback),
    lambda callback: engine.show_dialogue_with_options(
        "My PyRunner script is broken. Can you fix it so I can use it to run move_east() and escape fast enough to the desert?",
        options = {
            "Yes": lambda: engine.run_callback_list_sequence(player1_help_sequence),
            "No" : lambda: engine.run_callback_list_sequence(player1_reject_sequence)
        }
    ),

]


player1_help_sequence = [
    lambda callback: engine.show_dialogue("Thank you! Here is my script.",  callback = callback),
    lambda callback: player.set_busy(True, callback = callback),
    lambda callback: update_snake_stage(snake = 0, stage = 1, callback = callback),
    lambda callback: engine.show_external_script(
        confirm_callback = lambda: engine.run_callback_list_sequence(player1_try_script_sequence),
        cancel_callback = lambda: engine.run_callback_list_sequence(player1_cancel_script_sequence),
        external_dialogue = "Click 'Give Script' below when you're done.",
        script_init = lambda: engine.insert_to_scripter("moe_east()")),
    #lambda callback: engine.show_external_script_help("Give it a run when it's working!", callback = callback),
    #lambda callback: engine.show_dialogue("Test.",  callback = callback),
   # lambda callback: engine.hide_external_tab(callback = callback),
]

player1_reject_sequence = [
    lambda callback: engine.show_dialogue("I didn't think you'd be able to.", callback = callback),
    lambda callback: snake1.start_turning(time = 0.1, frequency = 8, callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
    #lambda callback: engine.wait(5.0, callback = callback),
    #lambda callback: player.set_busy(False, callback = callback),
]

def snake_finish_script(snake_index):

    update_snake_final_position(snake_index)

    engine.print_terminal(initialPosition[0])
    engine.print_terminal(initialPosition[1])
    snake1.run_script(script_to_run = 10, callback = snake_finish_script(0))#callback = lambda: engine.show_dialogue("Done with script!."))
    finalPosition = snake1.get_position()
    engine.print_terminal(finalPosition[0])
    engine.print_terminal(finalPosition[1])

def player1_run_script(callback = None):
        #initialPosition = snake1.get_position()
        #snakes_initial_pos[0] = initialPosition
        engine.print_terminal(initialPosition[0])
        engine.print_terminal(initialPosition[1])
        snake1.run_script(script_to_run = 10, callback = snake_finish_script(0))#callback = lambda: engine.show_dialogue("Done with script!."))
        finalPosition = snake1.get_position()
        engine.print_terminal(finalPosition[0])
        engine.print_terminal(finalPosition[1])
        callback()

player1_try_script_sequence = [
    lambda callback: engine.show_dialogue("Thank you, it means a lot! I'll try running it now.", callback = callback),
    #lambda callback: player.set_busy(False, callback = callback),
    lambda callback: update_snake_stage(snake = 0, stage = 0, callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
    lambda callback: player1_run_script(),
    #lambda callback: engine.print_terminal(engine.get_external_script()),
]

player1_cancel_script_sequence = [
    lambda callback: player.set_busy(False, callback = callback),
]


player1_complete_sequence = [
    lambda callback: engine.show_dialogue("Thank you! Here is my script. Please let me know when it works", callback = callback),
]


try_to_leave_sequence = [
    lambda callback: player.set_busy(True, callback = callback),
    lambda callback: engine.show_dialogue("Myla: Don't leave "+engine.get_player_name()+", they need our help!", callback = callback),
    #lambda callback: myla.stop_follow(callback = callback),
    #lambda callback: myla.move_east(callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
    lambda callback: player.move_east(callback = callback),
    #lambda callback: myla.follow(player, callback = callback)
]

myla_sequence = [
    lambda callback: engine.show_dialogue("I love you buddy.", callback = callback),
]

def snake1_action(player_object):
    if snake_helped[0] == 0:
        engine.run_callback_list_sequence(player1_sequence)
    elif snake_helped[0] == 1:
        engine.run_callback_list_sequence(player1_try_script_sequence)
    else:
        engine.run_callback_list_sequence(player1_complete)

snake1.player_action = snake1_action  #nake1_player_action

myla.player_action = lambda player_object: engine.run_callback_list_sequence(myla_sequence)



engine.run_callback_list_sequence(dialogue_sequence)

leave_hall.player_walked_on = lambda player_object: engine.run_callback_list_sequence(try_to_leave_sequence)

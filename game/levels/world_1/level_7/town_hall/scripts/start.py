import copy
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

#List of snakes
snakes = [snake1,snake2,snake3,snake4,snake5,snake6,snake7]
#List of initial and final snake positions (to check if they have move by correct amount)
snakes_initial_pos = []
snakes_final_pos = []

#Store whether you have correctly helped each snake yet
snakes_helped = []

#List of the action associated with each snake
#The snake which is talked to first gets 0
snake_actions = []

for snake in snakes:
    snakes_initial_pos.append((0,0))
    snakes_final_pos.append((0,0))
    snakes_helped.append(0)
    snake_actions.append(-1)
    snake.face_east()

snakes[0].change_state("orange")
snakes[1].change_state("green")
snakes[2].change_state("red")
snakes[3].change_state("green")
snakes[4].change_state("red")
snakes[5].change_state("yellow")
snakes[6].change_state("orange")

leader.change_state("blue")

#Get snake object from the action_index (ie 0 is the first snake talked to)
def get_snake(action_index):
    for count in range(len(snake_actions)):
        if (snake_actions[count] == action_index):
            snake = snakes[count]
            return snake

def get_snake_index(action_index):
    for count in range(len(snake_actions)):
        if (snake_actions[count] == action_index):
            snake = snakes[count]
            return count

def snakes_talked_to():
    talk_sum = 0

    for index in range(len(snakes)):
        if (snakes_helped[index] >= 1):
            talk_sum = talk_sum + 1

    return talk_sum


def update_snake_stage(snake = 0, stage = 1, callback = lambda: None):
    snakes_helped[snake] = stage
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

sequence_0 = [
    lambda callback: player.set_busy(True, callback = callback),
    lambda callback: get_snake(0).stop_turning(callback = callback),
    lambda callback: get_snake(0).turn_to_face(player, callback = callback),
    lambda callback: engine.show_dialogue("I'm scared. I thought the catchers were just a myth.", callback = callback),
    #lambda callback: engine.show_dialogue("My PyRunner script is broken. Can you fix it so I move east and get to the desert?", callback = callback),
    lambda callback: engine.show_dialogue_with_options(
        "My PyRunner script is broken. Can you fix it so I can use it to run move_east() and escape fast enough to the desert?",
        options = {
            "Yes": lambda: engine.run_callback_list_sequence(help_sequence_0),
            "No" : lambda: engine.run_callback_list_sequence(reject_sequence_0)
        }
    ),

]


help_sequence_0 = [
    lambda callback: engine.show_dialogue("Thank you! Here is my script.",  callback = callback),
    lambda callback: player.set_busy(True, callback = callback),
    #lambda callback: update_snake_stage(snake = 0, stage = 1, callback = callback),
    lambda callback: engine.show_external_script(
        confirm_callback = lambda: engine.run_callback_list_sequence(try_script_sequence_0),
        cancel_callback = lambda: engine.run_callback_list_sequence(cancel_script_sequence_0),
        external_dialogue = "Click 'Give Script' below when you're done.",
        script_init = lambda: engine.insert_to_scripter("moe_east()")),
    #lambda callback: engine.show_external_script_help("Give it a run when it's working!", callback = callback),
    #lambda callback: engine.show_dialogue("Test.",  callback = callback),
   # lambda callback: engine.hide_external_tab(callback = callback),
]

reject_sequence_0 = [
    lambda callback: engine.show_dialogue("I didn't think you'd be able to.", callback = callback),
    lambda callback: get_snake(0).start_turning(time = 0.1, frequency = 8, callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]

def snake_finish_script_0(snake_index):
    snake = snakes[snake_index]

    update_snake_final_position(snake_index)

    xchange = snakes_final_pos[snake_index][0] - snakes_initial_pos[snake_index][0]
    ychange = snakes_final_pos[snake_index][1] - snakes_initial_pos[snake_index][1]

    if ((engine.get_external_script()) == "move_east()" or (xchange == 1)):
        update_snake_stage(snake = snake_index, stage = 2)
        snake.turn_to_face(player)
        engine.run_callback_list_sequence(successful_sequence_0)
    else:
        engine.run_callback_list_sequence(unsuccessful_sequence_0)

def snake_run_script(action_index):
        snake = get_snake(action_index)
        snake_index = get_snake_index(action_index)

        initialPosition = snake.get_position()
        snakes_initial_pos[snake_index] = initialPosition

        snake.run_script(script_to_run = 10, parse_error = True, callback = lambda: snake_finish_script_0(snake_index))

try_script_sequence_0 = [
    lambda callback: engine.show_dialogue("Thank you, it means a lot! I'll try running it now.", callback = callback),
    #lambda callback: player.set_busy(False, callback = callback),
    lambda callback: update_snake_stage(snake = 0, stage = 0, callback = callback),
    lambda callback: player.set_busy(True, callback = callback),
    lambda callback: snake_run_script(0),
]

cancel_script_sequence_0 = [
    lambda callback: get_snake(0).start_turning(time = 0.1, frequency = 8, callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]

successful_sequence_0 = [
    lambda callback: engine.show_dialogue("Thank you so much, I can now move east!", callback = callback),
    lambda callback: myla.turn_to_face(player, callback = callback),
    lambda callback: engine.show_dialogue("Myla: Lets try and help the others!", callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]

unsuccessful_sequence_0 = [
    lambda callback: myla.wait(2, callback = callback),
    lambda callback: myla.turn_to_face(player, callback = callback),
    lambda callback: engine.show_dialogue("Myla: That didn't seem to work. Make sure their script says move_east() !", callback = callback),
    lambda callback: get_snake(0).start_turning(time = 0.1, frequency = 8, callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]


complete_sequence_0 = [
    lambda callback: engine.show_dialogue("Thank you for you help.", callback = callback),
]

sequence_1 = [
    lambda callback: engine.show_dialogue("Hi there2.", callback = callback),
]

complete_sequence_1 = [
    lambda callback: engine.show_dialogue("Thank you! Here is my script. Please let me know when it works", callback = callback),
]

sequence_2 = [
    lambda callback: engine.show_dialogue("Hi there3.", callback = callback),
]

complete_sequence_2 = [
    lambda callback: engine.show_dialogue("Thank you! Here is my script. Please let me know when it works", callback = callback),
]

sequence_3 = [
    lambda callback: engine.show_dialogue("Hi there4.", callback = callback),
]

complete_sequence_3 = [
    lambda callback: engine.show_dialogue("Thank you! Here is my script. Please let me know when it works", callback = callback),
]

sequence_4 = [
    lambda callback: engine.show_dialogue("Hi there5.", callback = callback),
]

complete_sequence_4 = [
    lambda callback: engine.show_dialogue("Thank you! Here is my script. Please let me know when it works", callback = callback),
]

sequence_5 = [
    lambda callback: engine.show_dialogue("Hi there6.", callback = callback),
]

complete_sequence_5 = [
    lambda callback: engine.show_dialogue("Thank you! Here is my script. Please let me know when it works", callback = callback),
]

sequence_6 = [
    lambda callback: engine.show_dialogue("Hi there7.", callback = callback),
]

complete_sequence_6 = [
    lambda callback: engine.show_dialogue("Thank you! Here is my script. Please let me know when it works", callback = callback),
]

sequence_7 = [
    lambda callback: engine.show_dialogue("Hi there8.", callback = callback),
]

complete_sequence_7 = [
    lambda callback: engine.show_dialogue("Thank you! Here is my script. Please let me know when it works", callback = callback),
]


try_to_leave_sequence = [
    lambda callback: player.set_busy(True, callback = callback),
    lambda callback: engine.show_dialogue("Myla: Don't leave "+engine.get_player_name()+", they need our help!", callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
    lambda callback: player.move_east(callback = callback),
    #lambda callback: myla.follow(player, callback = callback)
]

myla_sequence = [
    lambda callback: engine.show_dialogue("I love you buddy.", callback = callback),
]

#def snake1_action(player_object):
#    if snakes_helped[0] == 0:
#        engine.run_callback_list_sequence(player1_sequence)
#    elif snakes_helped[0] == 1:
#        engine.run_callback_list_sequence(player1_try_script_sequence)
#    else:
#        engine.run_callback_list_sequence(player1_complete)

def snake_intro(action_index):
    if (action_index == 0):
        engine.run_callback_list_sequence(sequence_0)
    elif (action_index == 1):
        engine.run_callback_list_sequence(sequence_1)
    elif (action_index == 2):
        engine.run_callback_list_sequence(sequence_2)
    elif (action_index == 3):
        engine.run_callback_list_sequence(sequence_3)
    elif (action_index == 4):
        engine.run_callback_list_sequence(sequence_4)
    elif (action_index == 5):
        engine.run_callback_list_sequence(sequence_5)
    elif (action_index == 6):
        engine.run_callback_list_sequence(sequence_6)

def snake_complete(action_index):
    if (action_index == 0):
        engine.run_callback_list_sequence(complete_sequence_0)
    elif (action_index == 1):
        engine.run_callback_list_sequence(complete_sequence_1)
    elif (action_index == 2):
        engine.run_callback_list_sequence(complete_sequence_2)
    elif (action_index == 3):
        engine.run_callback_list_sequence(complete_sequence_3)
    elif (action_index == 4):
        engine.run_callback_list_sequence(complete_sequence_4)
    elif (action_index == 5):
        engine.run_callback_list_sequence(complete_sequence_5)
    elif (action_index == 6):
        engine.run_callback_list_sequence(complete_sequence_6)

def snake_action(index):
    if snakes_helped[index] == 0:
        #Talk to first time
        snake_actions[index] = snakes_talked_to()
        snake_intro(snake_actions[index])
        snakes_helped[index] = 1
    elif snakes_helped[index] == 1:
        #Talk to again
        snake_intro(snake_actions[index])
    else:
        #Talk after helping
        snake_complete(snake_actions[index])


#for index in range(len(snakes)):
#    #current_index = copy.deepcopy(index)
#    #engine.print_terminal("current index is " + str(current_index))
#    #current_lambda = copy.deepcopy(lambda player_object: snake_action(current_index))
#    snakes[current_index].player_action = current_lambda

#for index in range(len(snakes)):
#    snakes[index].player_action = lambda player_object: snake_action(index)

snakes[0].player_action  = lambda player_object: snake_action(0)
snakes[1].player_action  = lambda player_object: snake_action(1)
snakes[2].player_action  = lambda player_object: snake_action(2)
snakes[3].player_action  = lambda player_object: snake_action(3)
snakes[4].player_action  = lambda player_object: snake_action(4)
snakes[5].player_action  = lambda player_object: snake_action(5)
snakes[6].player_action  = lambda player_object: snake_action(6)
#snakes[7].player_action  = lambda player_object: snake_action(7)

myla.player_action = lambda player_object: engine.run_callback_list_sequence(myla_sequence)

engine.run_callback_list_sequence(dialogue_sequence)

leave_hall.player_walked_on = lambda player_object: engine.run_callback_list_sequence(try_to_leave_sequence)

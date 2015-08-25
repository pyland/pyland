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



#List of snakes
snakes = [ben,robert,alexandra,annie,brad,francesca,jamie,elisabeth]
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

elisabeth.face_west()

snakes[0].change_state("yellow")
snakes[1].change_state("green")
snakes[2].change_state("red")
snakes[3].change_state("green")
snakes[4].change_state("orange")
snakes[5].change_state("orange")
snakes[6].change_state("red")
snakes[7].change_state("blue")

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


def turn_snakes(time = 0.1, callback = lambda: None):
    for snake_index in range(len(snakes)):
        if (not snakes_helped[snake_index] == 2):
            snakes[snake_index].start_turning(time, frequency = 8)
    elisabeth.stop_turning()
    callback()

####### CALL THIS AFTER EVERY SUCCESS, TO SEE IF TO TRIGGER END OF LEVEL
#def check_finish

pace_break = False

pace_stage = 0

def pace(cur_object, callback = lambda: None):
    global pace_break
    global pace_stage

    if (pace_break):
        return
    if (pace_stage == 0):
        cur_object.move_north(callback = lambda: pace(cur_object))
        pace_stage = pace_stage + 1
    elif (pace_stage == 1):
        cur_object.wait(1.0, callback = lambda: pace(cur_object))
        pace_stage = pace_stage + 1
    elif (pace_stage == 2):
        cur_object.move_south(callback = lambda: pace(cur_object))
        pace_stage = pace_stage + 1
    elif pace_stage == 3:
        cur_object.move_south(callback = lambda: pace(cur_object))
        pace_stage = pace_stage + 1
    elif pace_stage == 4:
        cur_object.wait(1.0, callback = lambda: pace(cur_object))
        pace_stage = pace_stage + 1
    elif pace_stage == 5:
        cur_object.move_north(callback = lambda: pace(cur_object))
        pace_stage = 0
    callback()
    #cur_object.move_north(lambda: cur_object.wait(1.0, lambda: cur_object.move_south(lambda: cur_object.move_south(lambda: cur_object.wait(1.0, lambda: cur_object.move_north(lambda: pace(cur_object)))))))




dialogue_sequence = [
    lambda callback: camera.move_to(player.get_position(), time = 0.0, callback = callback),
    lambda callback: camera.focus(callback = callback),
    lambda callback: camera.move_to(elisabeth.get_position(), time = 1.5, callback = callback),
    lambda callback: engine.show_dialogue("Minister: Attention my friends. I am sorry to have to say this again.", callback = callback),
    lambda callback: engine.show_dialogue("The catchers are back.", callback = callback),
    lambda callback: turn_snakes(callback = callback),
    lambda callback: camera.move_to(jamie.get_position(), time = 1.25, callback = callback),
    lambda callback: engine.show_dialogue("Jamie: But we don't have running scripts to properly escape!", callback = callback),
    lambda callback: elisabeth.move_north(callback = callback),
    lambda callback: elisabeth.move_west(callback = callback),
    lambda callback: elisabeth.move_west(callback = callback),
    lambda callback: elisabeth.move_south(callback = callback),
    lambda callback: elisabeth.face_west(callback = callback),
    lambda callback: engine.show_dialogue("Please stay calm. I can handle this.", callback = callback),
    lambda callback: engine.show_dialogue("But we must prepare to head to the desert, even if we cannot use our scripts to escape quickly.", callback = callback),
    lambda callback: myla.move_east(callback = callback),
    lambda callback: camera.move_to(player.get_position(), time = 1.5, callback = callback),
    lambda callback: player.focus(callback = callback),
    lambda callback: engine.show_dialogue("Myla: Lets help them out, talk to as many of them as you can.", callback = callback),
    lambda callback: pace(elisabeth, callback = callback),
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
        external_dialogue = "Click 'Give Script' below when you're done. It should move me east one step.",
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

def snake_run_script(action_index, run_callback = lambda: None):
        snake = get_snake(action_index)
        snake_index = get_snake_index(action_index)

        initialPosition = snake.get_position()
        snakes_initial_pos[snake_index] = initialPosition

        snake.run_script(script_to_run = 10, parse_error = True, callback = lambda: run_callback(snake_index))

try_script_sequence_0 = [
    lambda callback: engine.show_dialogue("Thank you, it means a lot! I'll try running it now.", callback = callback),
    #lambda callback: player.set_busy(False, callback = callback),
    lambda callback: player.set_busy(True, callback = callback),
    lambda callback: snake_run_script(0, run_callback = snake_finish_script_0),
]

cancel_script_sequence_0 = [
    lambda callback: get_snake(0).start_turning(time = 0.1, frequency = 8, callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]

def turn_snakes_west(callback = lambda: None):
    for snake in snakes:
        snake.stop_turning()
        snake.face_west()
    callback()

#Stop the minister pacing
def stop_pace(callback = lambda: None):
    global pace_break
    pace_break = True
    callback()

def all_face_leader(callback = lambda: None):
    #don't turn leader
    for snake_index in range(len(snakes)-1):
        snakes[snake_index].turn_to_face(elisabeth)
    myla.turn_to_face(elisabeth)
    player.turn_to_face(elisabeth)
    callback()


successful_sequence_0 = [
    lambda callback: engine.show_dialogue("Thank you so much, I can now move east!", callback = callback),
    lambda callback: myla.turn_to_face(player, callback = callback),
    lambda callback: engine.show_dialogue("Myla: Lets try and help the others!", callback = callback),
    #lambda callback: player.set_busy(False, callback = callback),
    lambda callback: myla.wait(0.5, callback = callback),
    #lambda callback: engine.show_dialogue("Test !", callback = callback),
    #lambda callback: camera.move_to(player.get_position(), time = 0.0, callback = callback),
    lambda callback: camera.move_to(leave_hall.get_position(), time = 0.0, callback = callback),
    #lambda callback: engine.show_dialogue("Test2 !", callback = callback),
    lambda callback: camera.focus(callback = callback),
    #lambda callback: engine.show_dialogue("Test22 !", callback = callback),
    lambda callback: camera.move_by((3,0), time = 0.15, callback = callback),
    lambda callback: camera.move_by((-3,0), time = 0.15, callback = callback),
    lambda callback: myla.wait(0.5, callback = callback),
    lambda callback: myla.stop_follow(callback = callback),
    lambda callback: turn_snakes_west(callback = callback),
    lambda callback: player.face_west(callback = callback),
    lambda callback: myla.face_west(callback = callback),
    lambda callback: stop_pace(callback = callback),
    lambda callback: camera.move_by((3,0), time = 0.15, callback = callback),
    lambda callback: camera.move_by((-3,0), time = 0.15, callback = callback),
    lambda callback: engine.show_dialogue("THUD... THUD...", callback = callback),
    lambda callback: camera.move_to(elisabeth.get_position(), time = 1.0, callback = callback),
    lambda callback: engine.show_dialogue("Minister: They're here...", callback = callback),
    lambda callback: all_face_leader(callback = callback),
    lambda callback: engine.show_dialogue("Let me handle this... Stay calm my friends, you're safe locked in here...", callback = callback),
    lambda callback: elisabeth.start_animating(speed = 0.06, loop = True, forward = True, callback = callback),
    lambda callback: elisabeth.move_to(leave_hall.get_position(), time = 1.75, callback = callback),
    lambda callback: all_face_leader(callback = callback),
    lambda callback: elisabeth.set_visible(False, callback = callback),
    lambda callback: elisabeth.stop_animating(callback = callback),
    lambda callback: camera.move_to(player.get_position(), time = 2.0, callback = callback),
    lambda callback: turn_snakes(time = 0.075, callback = callback),
    lambda callback: myla.face_east(callback = callback),
    lambda callback: engine.show_dialogue("Myla: "+engine.get_player_name()+" we must make sure everyone's scripts are working!", callback = callback),
    lambda callback: player.focus(callback = callback),
    lambda callback: player.set_busy(False, callback = callback),

]


unsuccessful_sequence_0 = [
    lambda callback: myla.wait(1.5, callback = callback),
    lambda callback: myla.turn_to_face(player, callback = callback),
    lambda callback: engine.show_dialogue("Myla: That didn't seem to work. Make sure their script says move_east() !", callback = callback),
    lambda callback: get_snake(0).start_turning(time = 0.1, frequency = 8, callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]


complete_sequence_0 = [
    lambda callback: engine.show_dialogue("Thank you so much for fixing my script. I feel safe now!", callback = callback),
]

sequence_1 = [
    lambda callback: player.set_busy(True, callback = callback),
    lambda callback: get_snake(1).stop_turning(callback = callback),
    lambda callback: get_snake(1).turn_to_face(player, callback = callback),
    lambda callback: engine.show_dialogue("My north movement script is broken. I need it to get around the buildings, if we run to the desert!", callback = callback),
    lambda callback: engine.run_callback_list_sequence(help_sequence_1),
]

help_sequence_1 = [
    lambda callback: engine.show_external_script(
        confirm_callback = lambda: engine.run_callback_list_sequence(try_script_sequence_1),
        cancel_callback = lambda: engine.run_callback_list_sequence(cancel_script_sequence_1),
        external_dialogue = "Here's my script. Please try and fix it then choose 'Give Script'.",
        script_init = lambda: engine.insert_to_scripter("move_()")),
]

def snake_finish_script_1(snake_index):
    snake = snakes[snake_index]

    update_snake_final_position(snake_index)

    xchange = snakes_final_pos[snake_index][0] - snakes_initial_pos[snake_index][0]
    ychange = snakes_final_pos[snake_index][1] - snakes_initial_pos[snake_index][1]

    engine.print_terminal("snake initial pos "+str(snakes_initial_pos[snake_index]),False)
    engine.print_terminal("snake final pos "+str(snakes_final_pos[snake_index]),False)
    engine.print_terminal("xchange is "+str(xchange),False)
    engine.print_terminal("ychange is "+str(ychange),False)

    if ((engine.get_external_script()) == "move_north()" or (ychange == 1)):
        update_snake_stage(snake = snake_index, stage = 2)
        snake.turn_to_face(player)
        engine.run_callback_list_sequence(successful_sequence_1)
    else:
        engine.run_callback_list_sequence(unsuccessful_sequence_1)

successful_sequence_1 = [
    lambda callback: engine.show_dialogue("Perfect, it works!", callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]


unsuccessful_sequence_1 = [
    lambda callback: myla.wait(1.5, callback = callback),
    lambda callback: myla.turn_to_face(player, callback = callback),
    lambda callback: engine.show_dialogue("Myla: I think the script needs to say move_north() !", callback = callback),
    lambda callback: get_snake(1).start_turning(time = 0.1, frequency = 8, callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]

try_script_sequence_1 = [
    lambda callback: engine.show_dialogue("Thanks for having tinker with it! Lets give this a go...", callback = callback),
    #lambda callback: player.set_busy(False, callback = callback),
    lambda callback: snake_run_script(1, run_callback = snake_finish_script_1),
]

cancel_script_sequence_1 = [
    lambda callback: get_snake(1).start_turning(time = 0.1, frequency = 8, callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]



complete_sequence_1 = [
    lambda callback: engine.show_dialogue("You and your monkey friend are so awesome!", callback = callback),
]

sequence_2 = [
    lambda callback: player.set_busy(True, callback = callback),
    lambda callback: get_snake(2).stop_turning(callback = callback),
    lambda callback: get_snake(2).turn_to_face(player, callback = callback),
    lambda callback: engine.show_dialogue("My PyRunner doesn't seem to know my move east script is calling a function!", callback = callback),
    lambda callback: engine.show_external_script(
        confirm_callback = lambda: engine.run_callback_list_sequence(try_script_sequence_2),
        cancel_callback = lambda: engine.run_callback_list_sequence(cancel_script_sequence_2),
        external_dialogue = "I'm not sure how to make sure it knows to call the function to move east...",
        script_init = lambda: engine.insert_to_scripter("move_east")),
]

def snake_finish_script_2(snake_index):
    snake = snakes[snake_index]

    update_snake_final_position(snake_index)

    xchange = snakes_final_pos[snake_index][0] - snakes_initial_pos[snake_index][0]
    ychange = snakes_final_pos[snake_index][1] - snakes_initial_pos[snake_index][1]

    engine.print_terminal("snake initial pos "+str(snakes_initial_pos[snake_index]),False)
    engine.print_terminal("snake final pos "+str(snakes_final_pos[snake_index]),False)
    engine.print_terminal("xchange is "+str(xchange),False)
    engine.print_terminal("ychange is "+str(ychange),False)

    if ((engine.get_external_script()) == "move_east()" or (xchange == 1)):
        update_snake_stage(snake = snake_index, stage = 2)
        snake.turn_to_face(player)
        engine.run_callback_list_sequence(successful_sequence_2)
    else:
        engine.run_callback_list_sequence(unsuccessful_sequence_2)

successful_sequence_2 = [
    lambda callback: engine.show_dialogue("Perfect, it works!", callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]


unsuccessful_sequence_2 = [
    lambda callback: myla.wait(1.5, callback = callback),
    lambda callback: myla.turn_to_face(player, callback = callback),
    lambda callback: engine.show_dialogue("Myla: I think the script needs to say move_east() !", callback = callback),
    lambda callback: get_snake(1).start_turning(time = 0.1, frequency = 8, callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]

try_script_sequence_2 = [
    lambda callback: engine.show_dialogue("Thanks for having tinker with it! Lets give this a go...", callback = callback),
    #lambda callback: player.set_busy(False, callback = callback),
    lambda callback: snake_run_script(2, run_callback = snake_finish_script_2),
]

cancel_script_sequence_2 = [
    lambda callback: get_snake(2).start_turning(time = 0.1, frequency = 8, callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]

complete_sequence_2 = [
    lambda callback: engine.show_dialogue("Thank you! Here is my script. Please let me know when it works", callback = callback),
]

sequence_3 = [
    lambda callback: player.set_busy(True, callback = callback),
    lambda callback: get_snake(2).stop_turning(callback = callback),
    lambda callback: get_snake(2).turn_to_face(player, callback = callback),
    lambda callback: engine.show_dialogue("My move_east loop is broken!", callback = callback),
    lambda callback: engine.show_external_script(
        confirm_callback = lambda: engine.run_callback_list_sequence(try_script_sequence_2),
        cancel_callback = lambda: engine.run_callback_list_sequence(cancel_script_sequence_2),
        external_dialogue = "I'm not sure how to make sure it knows to call the function to move east...",
        script_init = lambda: engine.insert_to_scripter("move_east")),
]

def snake_finish_script_3(snake_index):
    snake = snakes[snake_index]

    update_snake_final_position(snake_index)

    xchange = snakes_final_pos[snake_index][0] - snakes_initial_pos[snake_index][0]
    ychange = snakes_final_pos[snake_index][1] - snakes_initial_pos[snake_index][1]

    engine.print_terminal("snake initial pos "+str(snakes_initial_pos[snake_index]),False)
    engine.print_terminal("snake final pos "+str(snakes_final_pos[snake_index]),False)
    engine.print_terminal("xchange is "+str(xchange),False)
    engine.print_terminal("ychange is "+str(ychange),False)

    if ((engine.get_external_script()) == "move_north()" or (ychange == 1)):
        update_snake_stage(snake = snake_index, stage = 2)
        snake.turn_to_face(player)
        engine.run_callback_list_sequence(successful_sequence_3)
    else:
        engine.run_callback_list_sequence(unsuccessful_sequence_3)

successful_sequence_3 = [
    lambda callback: engine.show_dialogue("Perfect, it works!", callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]


unsuccessful_sequence_3 = [
    lambda callback: myla.wait(1.5, callback = callback),
    lambda callback: myla.turn_to_face(player, callback = callback),
    lambda callback: engine.show_dialogue("Myla: I think the script needs to say move_north() !", callback = callback),
    lambda callback: get_snake(1).start_turning(time = 0.1, frequency = 8, callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]

try_script_sequence_3 = [
    lambda callback: engine.show_dialogue("Thanks for having tinker with it! Lets give this a go...", callback = callback),
    #lambda callback: player.set_busy(False, callback = callback),
    lambda callback: snake_run_script(3, run_callback = snake_finish_script_2),
]

cancel_script_sequence_3 = [
    lambda callback: get_snake(3).start_turning(time = 0.1, frequency = 8, callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
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

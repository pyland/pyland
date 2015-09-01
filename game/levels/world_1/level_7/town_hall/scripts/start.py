import copy
from random import randint
import string

world_name = "world_1"
level_name = "level_7"
map_name = "town_hall"

engine.update_world_text("1")
engine.update_level_text("7")

player_data.load(engine.get_player_name())
player_data.set_map(world_name, level_name = level_name, map_name = map_name)

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
    for snake in snakes:
        snake.stop_turning()
    for snake_index in range(len(snakes)):
        if (not snakes_helped[snake_index] == 2):
            snakes[snake_index].start_turning(time, frequency = 8)
    elisabeth.stop_turning()
    callback()


def level_finished():
    finished = True
    for snake_index in range(len(snakes)-1):
        if (not snakes_helped[snake_index] == 2):
            finished = False
    return finished

def check_finish():
    if level_finished():
        engine.run_callback_list_sequence(finish_sequence)



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
    lambda callback: camera.move_to(brad.get_position(), time = 1.25, callback = callback),
    lambda callback: engine.show_dialogue("Brad: But we don't have running scripts to properly escape!", callback = callback),
    lambda callback: elisabeth.move_north(callback = callback),
    lambda callback: elisabeth.move_west(callback = callback),
    lambda callback: elisabeth.move_west(callback = callback),
    lambda callback: elisabeth.move_south(callback = callback),
    lambda callback: elisabeth.face_west(callback = callback),
    lambda callback: camera.move_to(elisabeth.get_position(), time = 0.75, callback = callback),
    lambda callback: engine.show_dialogue("Minister: Please stay calm. I can handle this.", callback = callback),
    lambda callback: engine.show_dialogue("But we must prepare to head to the desert, even if we cannot use our scripts to escape quickly.", callback = callback),
    lambda callback: myla.move_east(callback = callback),
    lambda callback: camera.move_to(player.get_position(), time = 1.5, callback = callback),
    lambda callback: player.focus(callback = callback),
    lambda callback: engine.show_dialogue("Myla: Lets help them out, we might be able to fix their scripts! Talk to as many of them as you can.", callback = callback),
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
    lambda callback: engine.show_external_script(
        confirm_callback = lambda: engine.run_callback_list_sequence(try_script_sequence_0),
        cancel_callback = lambda: engine.run_callback_list_sequence(cancel_script_sequence_0),
        external_dialogue = "Click 'Give Script' below when you're done. It should move me east one step.",
        script_init = lambda: engine.insert_to_scripter("moe_east()")),
]

reject_sequence_0 = [
    lambda callback: engine.show_dialogue("I didn't think it was possible to edit scripts.", callback = callback),
    lambda callback: get_snake(0).start_turning(time = 0.1, frequency = 8, callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]

def snake_finish_script_0(snake_index):
    snake = snakes[snake_index]

    update_snake_final_position(snake_index)

    xchange = snakes_final_pos[snake_index][0] - snakes_initial_pos[snake_index][0]
    ychange = snakes_final_pos[snake_index][1] - snakes_initial_pos[snake_index][1]

    removed_white_space = (engine.get_external_script()).replace(" ","").replace("\n","").replace("  ","")

    set_all_solid()

    if (removed_white_space == "move_east()" or ((xchange == 1) and (ychange == 0))) and (not engine.get_error()):
        update_snake_stage(snake = snake_index, stage = 2)
        snake.turn_to_face(player)
        engine.run_callback_list_sequence(successful_sequence_0, callback = check_finish)
    else:
        engine.run_callback_list_sequence(unsuccessful_sequence_0)

def set_other_snakes_not_solid(snake_index):
    for cur_index in range(len(snakes)):
        if not snake_index == cur_index:
            snakes[cur_index].set_solidity(False)

def set_all_solid():
    for snake in snakes:
        snake.set_solidity(True)
    player.set_solidity(True)


def snake_run_script(action_index, run_callback = lambda: None):
        snake = get_snake(action_index)
        snake_index = get_snake_index(action_index)

        initialPosition = snake.get_position()
        snakes_initial_pos[snake_index] = initialPosition

        removed_white_space = (engine.get_external_script()).replace(" ","").replace("\n","").replace("  ","")

        player.set_solidity(False)

        #set all other snakes to not solid

        set_other_snakes_not_solid(snake_index)


        snake.set_solidity(True, callback = lambda: snake.run_script(script_to_run = 10, parse_error = True, callback = lambda: run_callback(snake_index)))
        snake.wait(10.0, callback = snake.halt_script)

try_script_sequence_0 = [
    lambda callback: engine.show_dialogue("Thank you, this means a lot! I'll try running it now.", callback = callback),
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
    lambda callback: engine.show_dialogue("You did it! I can now move east!", callback = callback),
    lambda callback: myla.turn_to_face(player, callback = callback),
    lambda callback: engine.show_dialogue("Myla: Lets try and help the others!", callback = callback),
    lambda callback: myla.wait(0.5, callback = callback),
    lambda callback: camera.move_to(leave_hall.get_position(), time = 0.0, callback = callback),
    lambda callback: camera.focus(callback = callback),
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
    lambda callback: engine.show_dialogue("Let me handle this... Stay calm my friends, you should be safe in this locked building...", callback = callback),
    lambda callback: elisabeth.start_animating(speed = 0.06, loop = True, forward = True, callback = callback),
    lambda callback: elisabeth.move_to(leave_hall.get_position(), time = 1.75, callback = callback),
    lambda callback: all_face_leader(callback = callback),
    lambda callback: elisabeth.set_visible(False, callback = callback),
    lambda callback: elisabeth.set_solidity(False, callback = callback),
    lambda callback: elisabeth.stop_animating(callback = callback),
    lambda callback: camera.move_to(player.get_position(), time = 2.0, callback = callback),
    lambda callback: turn_snakes(time = 0.075, callback = callback),
    lambda callback: myla.face_east(callback = callback),
    lambda callback: engine.show_dialogue("Myla: "+engine.get_player_name()+" we must make sure everyone's scripts are working!", callback = callback),
    lambda callback: player.focus(callback = callback),
    lambda callback: myla.follow(player, callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]

unsuccessful_sequence_0 = [
    lambda callback: myla.wait(1.5, callback = callback),
    lambda callback: myla.turn_to_face(player, callback = callback),
    lambda callback: engine.show_dialogue("Myla: That didn't seem to work. Talk to them again and make sure their script says move_east()", callback = callback),
    lambda callback: engine.show_dialogue("... it needs to work without errors!", callback = callback),
    lambda callback: get_snake(0).start_turning(time = 0.1, frequency = 8, callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]


complete_sequence_0 = [
    lambda callback: player.set_busy(True, callback = callback),
    lambda callback: get_snake(0).turn_to_face(player, callback = callback),
    lambda callback: engine.show_dialogue("Thank you so much for fixing my script. I feel safe now!", callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]

sequence_1 = [
    lambda callback: player.set_busy(True, callback = callback),
    lambda callback: get_snake(1).stop_turning(callback = callback),
    lambda callback: get_snake(1).turn_to_face(player, callback = callback),
    lambda callback: engine.show_dialogue("My west movement script is broken. I might need it to avoid the catchers, if we are to run to the desert!", callback = callback),
    lambda callback: engine.run_callback_list_sequence(help_sequence_1),
]

help_sequence_1 = [
    lambda callback: engine.show_external_script(
        confirm_callback = lambda: engine.run_callback_list_sequence(try_script_sequence_1),
        cancel_callback = lambda: engine.run_callback_list_sequence(cancel_script_sequence_1),
        external_dialogue = "Here is my script. It would be great if you could fix it to make me go west and then choose 'Give Script'.",
        script_init = lambda: engine.insert_to_scripter("move_()")),
]

def snake_finish_script_1(snake_index):
    snake = snakes[snake_index]

    update_snake_final_position(snake_index)

    xchange = snakes_final_pos[snake_index][0] - snakes_initial_pos[snake_index][0]
    ychange = snakes_final_pos[snake_index][1] - snakes_initial_pos[snake_index][1]

    set_all_solid()

    removed_white_space = (engine.get_external_script()).replace(" ","").replace("\n","").replace("  ","")

    if (removed_white_space == "move_west()" or ((xchange == -1)  and (ychange == 0))) and (not engine.get_error()):
        update_snake_stage(snake = snake_index, stage = 2)
        snake.turn_to_face(player)
        engine.run_callback_list_sequence(successful_sequence_1, callback = check_finish)
    else:
        engine.run_callback_list_sequence(unsuccessful_sequence_1)

successful_sequence_1 = [
    lambda callback: engine.show_dialogue("Perfect, it works!", callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]


unsuccessful_sequence_1 = [
    lambda callback: myla.wait(1.5, callback = callback),
    lambda callback: myla.turn_to_face(player, callback = callback),
    lambda callback: engine.show_dialogue("Myla: I think the script needs to say move_west()", callback = callback),
    lambda callback: get_snake(1).start_turning(time = 0.1, frequency = 8, callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]

try_script_sequence_1 = [
    lambda callback: engine.show_dialogue("Lets give this a go...", callback = callback),
    lambda callback: snake_run_script(1, run_callback = snake_finish_script_1),
]

cancel_script_sequence_1 = [
    lambda callback: get_snake(1).start_turning(time = 0.1, frequency = 8, callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]

complete_sequence_1 = [
    lambda callback: player.set_busy(True, callback = callback),
    lambda callback: get_snake(1).turn_to_face(player, callback = callback),
    lambda callback: engine.show_dialogue("You and your monkey friend are so awesome!", callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]

sequence_2 = [
    lambda callback: player.set_busy(True, callback = callback),
    lambda callback: get_snake(2).stop_turning(callback = callback),
    lambda callback: get_snake(2).turn_to_face(player, callback = callback),
    lambda callback: engine.show_dialogue("My PyRunner doesn't seem to understand my move east script!", callback = callback),
    lambda callback: engine.show_external_script(
        confirm_callback = lambda: engine.run_callback_list_sequence(try_script_sequence_2),
        cancel_callback = lambda: engine.run_callback_list_sequence(cancel_script_sequence_2),
        external_dialogue = "Perhaps it doesn't realise move east is a function...",
        script_init = lambda: engine.insert_to_scripter("move_east")),
]

def snake_finish_script_2(snake_index):
    snake = snakes[snake_index]

    update_snake_final_position(snake_index)

    xchange = snakes_final_pos[snake_index][0] - snakes_initial_pos[snake_index][0]
    ychange = snakes_final_pos[snake_index][1] - snakes_initial_pos[snake_index][1]

    set_all_solid()

    removed_white_space = (engine.get_external_script()).replace(" ","").replace("\n","").replace("  ","")

    if (removed_white_space == "move_east()" or ((xchange == 1)  and (ychange == 0))) and (not engine.get_error()):
        update_snake_stage(snake = snake_index, stage = 2)
        snake.turn_to_face(player)
        engine.run_callback_list_sequence(successful_sequence_2, callback = check_finish)
    else:
        engine.run_callback_list_sequence(unsuccessful_sequence_2)

successful_sequence_2 = [
    lambda callback: engine.show_dialogue("It works! Those brackets seem to make it understand that it's calling a function.", callback = callback),
    lambda callback: engine.show_dialogue("I guess it thought move_east was variable!", callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]


unsuccessful_sequence_2 = [
    lambda callback: myla.wait(1.5, callback = callback),
    lambda callback: myla.turn_to_face(player, callback = callback),
    lambda callback: engine.show_dialogue("Myla: I think the script needs to say move_east() ! The brackets seem important.", callback = callback),
    lambda callback: get_snake(2).start_turning(time = 0.1, frequency = 8, callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]

try_script_sequence_2 = [
    lambda callback: engine.show_dialogue("This might work!", callback = callback),
    lambda callback: snake_run_script(2, run_callback = snake_finish_script_2),
]

cancel_script_sequence_2 = [
    lambda callback: get_snake(2).start_turning(time = 0.1, frequency = 8, callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]

complete_sequence_2 = [
    lambda callback: player.set_busy(True, callback = callback),
    lambda callback: get_snake(2).turn_to_face(player, callback = callback),
    lambda callback: engine.show_dialogue("I love living in the jungle.", callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]

sequence_3 = [
    lambda callback: player.set_busy(True, callback = callback),
    lambda callback: get_snake(3).stop_turning(callback = callback),
    lambda callback: get_snake(3).turn_to_face(player, callback = callback),
    lambda callback: engine.show_dialogue("Someone thought it would be funny to give me script that moves me east and then back again!", callback = callback),
    lambda callback: engine.show_external_script(
        confirm_callback = lambda: engine.run_callback_list_sequence(try_script_sequence_3),
        cancel_callback = lambda: engine.run_callback_list_sequence(cancel_script_sequence_3),
        external_dialogue = "Can you make it just move me east by one space?",
        script_init = lambda: engine.insert_to_scripter("move_east()\nmove_west()")),
]

def snake_finish_script_3(snake_index):
    snake = snakes[snake_index]

    update_snake_final_position(snake_index)

    xchange = snakes_final_pos[snake_index][0] - snakes_initial_pos[snake_index][0]
    ychange = snakes_final_pos[snake_index][1] - snakes_initial_pos[snake_index][1]

    set_all_solid()

    removed_white_space = (engine.get_external_script()).replace(" ","").replace("\n","").replace("  ","")

    if (removed_white_space == "move_east()" or ((xchange == 1) and (ychange == 0))) and (not engine.get_error()):
        update_snake_stage(snake = snake_index, stage = 2)
        snake.turn_to_face(player)
        engine.run_callback_list_sequence(successful_sequence_3, callback = check_finish)
    else:
        engine.run_callback_list_sequence(unsuccessful_sequence_3)

successful_sequence_3 = [
    lambda callback: engine.show_dialogue("Perfect, it works!", callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]


unsuccessful_sequence_3 = [
    lambda callback: myla.wait(1.5, callback = callback),
    lambda callback: myla.turn_to_face(player, callback = callback),
    lambda callback: engine.show_dialogue("Myla: Make sure the script only contains move_east()", callback = callback),
    lambda callback: get_snake(3).start_turning(time = 0.1, frequency = 8, callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]

try_script_sequence_3 = [
    lambda callback: engine.show_dialogue("Thanks for having tinker with it! Lets give this a go...", callback = callback),
    #lambda callback: player.set_busy(False, callback = callback),
    lambda callback: snake_run_script(3, run_callback = snake_finish_script_3),
]

cancel_script_sequence_3 = [
    lambda callback: get_snake(3).start_turning(time = 0.1, frequency = 8, callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]


complete_sequence_3 = [
    lambda callback: player.set_busy(True, callback = callback),
    lambda callback: get_snake(3).turn_to_face(player, callback = callback),
    lambda callback: engine.show_dialogue("I'm learning to play the marimba. I hope the catchers don't steal anything from my house!", callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]

sequence_4 = [
    lambda callback: player.set_busy(True, callback = callback),
    lambda callback: get_snake(4).stop_turning(callback = callback),
    lambda callback: get_snake(4).turn_to_face(player, callback = callback),
    lambda callback: engine.show_dialogue("I knew it by was bad idea to get a script that tries to print to my PyRunner terminal!", callback = callback),
    lambda callback: engine.show_external_script(
        confirm_callback = lambda: engine.run_callback_list_sequence(try_script_sequence_4),
        cancel_callback = lambda: engine.run_callback_list_sequence(cancel_script_sequence_4),
        external_dialogue = "The print command is broken so it won't move east!",
        script_init = lambda: engine.insert_to_scripter("pr int('I am moving east')\nmove_east()")),
]

def snake_finish_script_4(snake_index):
    snake = snakes[snake_index]

    update_snake_final_position(snake_index)

    xchange = snakes_final_pos[snake_index][0] - snakes_initial_pos[snake_index][0]
    ychange = snakes_final_pos[snake_index][1] - snakes_initial_pos[snake_index][1]

    set_all_solid()

    removed_white_space = (engine.get_external_script()).replace(" ","").replace("\n","").replace("  ","")

    if (removed_white_space == "move_east()" or removed_white_space == "print('I'm moving east')move_east()" or ((xchange == 1) and (ychange == 0))) and (not engine.get_error()):
        update_snake_stage(snake = snake_index, stage = 2)
        snake.turn_to_face(player)
        engine.run_callback_list_sequence(successful_sequence_4, callback = check_finish)
    else:
        engine.run_callback_list_sequence(unsuccessful_sequence_4)

successful_sequence_4 = [
    lambda callback: engine.show_dialogue("Great, it no longer crashes!", callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]


unsuccessful_sequence_4 = [
    lambda callback: myla.wait(1.5, callback = callback),
    lambda callback: myla.turn_to_face(player, callback = callback),
    lambda callback: engine.show_dialogue("Myla: Try fixing the print statment, or removing it!", callback = callback),
    lambda callback: get_snake(4).start_turning(time = 0.1, frequency = 8, callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]

try_script_sequence_4 = [
    lambda callback: engine.show_dialogue("Lets see if this works.", callback = callback),
    lambda callback: snake_run_script(4, run_callback = snake_finish_script_4),
]

cancel_script_sequence_4 = [
    lambda callback: get_snake(4).start_turning(time = 0.1, frequency = 8, callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]

complete_sequence_4 = [
    lambda callback: player.set_busy(True, callback = callback),
    lambda callback: get_snake(4).turn_to_face(player, callback = callback),
    lambda callback: engine.show_dialogue("I've heard rumours that beyond the desert is a vast span of water!", callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]

sequence_5 = [
    lambda callback: player.set_busy(True, callback = callback),
    lambda callback: get_snake(5).stop_turning(callback = callback),
    lambda callback: get_snake(5).turn_to_face(player, callback = callback),
    lambda callback: engine.show_dialogue("I can move east, but then I can't stop and I can't halt my RyRunner!", callback = callback),
    lambda callback: engine.show_external_script(
        confirm_callback = lambda: engine.run_callback_list_sequence(try_script_sequence_5),
        cancel_callback = lambda: engine.run_callback_list_sequence(cancel_script_sequence_5),
        external_dialogue = "I think the while loop needs removing. ",
        script_init = lambda: engine.insert_to_scripter("while True:\n  move_east()")),
]

def snake_finish_script_5(snake_index):
    snake = snakes[snake_index]

    update_snake_final_position(snake_index)

    xchange = snakes_final_pos[snake_index][0] - snakes_initial_pos[snake_index][0]
    ychange = snakes_final_pos[snake_index][1] - snakes_initial_pos[snake_index][1]

    set_all_solid()

    removed_white_space = (engine.get_external_script()).replace(" ","").replace("\n","").replace("  ","")

    if (removed_white_space == "move_east()" or ((xchange == 1) and (ychange == 0))) and (not engine.get_error()):
        update_snake_stage(snake = snake_index, stage = 2)
        snake.turn_to_face(player)
        engine.run_callback_list_sequence(successful_sequence_5, callback = check_finish)
    else:
        engine.run_callback_list_sequence(unsuccessful_sequence_5)

successful_sequence_5 = [
    lambda callback: engine.show_dialogue("Yes, it's working! Fixing the problem with scripting is way cooler than using halt!", callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]


unsuccessful_sequence_5 = [
    lambda callback: myla.wait(1.5, callback = callback),
    lambda callback: myla.turn_to_face(player, callback = callback),
    lambda callback: engine.show_dialogue("Myla: Looks like didn't work. Make sure you remove the tab from the start of move_east(), when it's no longer inside a loop!", callback = callback),
    lambda callback: engine.show_dialogue("Good job I set their scripts to terminate after 10 seconds, or they could get stuck forever!", callback = callback),

    lambda callback: get_snake(5).start_turning(time = 0.1, frequency = 8, callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]

try_script_sequence_5 = [
    lambda callback: engine.show_dialogue("Lets hope I don't get stuck in a loop!", callback = callback),
    lambda callback: snake_run_script(5, run_callback = snake_finish_script_5),
]

cancel_script_sequence_5 = [
    lambda callback: get_snake(5).start_turning(time = 0.1, frequency = 8, callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]

complete_sequence_5 = [
    lambda callback: player.set_busy(True, callback = callback),
    lambda callback: get_snake(5).turn_to_face(player, callback = callback),
    lambda callback: engine.show_dialogue("I'm free from the burden of infinite loops!", callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]

sequence_6 = [
    lambda callback: player.set_busy(True, callback = callback),
    lambda callback: get_snake(6).stop_turning(callback = callback),
    lambda callback: get_snake(6).turn_to_face(player, callback = callback),
    lambda callback: engine.show_dialogue("I'm fortunate enough that all my scripts work! I think it's so cool that you can write your own scripts.", callback = callback),
    lambda callback: engine.show_dialogue("Oh I know something fun!", callback = callback),
    lambda callback: engine.show_external_script(
        confirm_callback = lambda: engine.run_callback_list_sequence(try_script_sequence_6),
        cancel_callback = lambda: engine.run_callback_list_sequence(cancel_script_sequence_6),
        external_dialogue = "Print me your best joke!",
        script_init = lambda: engine.insert_to_scripter("")),
]

def snake_finish_script_6(snake_index):
    snake = snakes[snake_index]

    update_snake_final_position(snake_index)

    xchange = snakes_final_pos[snake_index][0] - snakes_initial_pos[snake_index][0]
    ychange = snakes_final_pos[snake_index][1] - snakes_initial_pos[snake_index][1]

    set_all_solid()

    if ((not engine.get_error()) and ("print" in engine.get_external_script())):
        snake.turn_to_face(player)
        update_snake_stage(snake = snake_index, stage = 2)
        response =randint(0,2)
        if response == 0:
            engine.run_callback_list_sequence(successful_sequence_6_1, callback = check_finish)
        elif response == 1:
            engine.run_callback_list_sequence(successful_sequence_6_2, callback = check_finish)
        else:
            engine.run_callback_list_sequence(successful_sequence_6_3, callback = check_finish)

    else:
        engine.run_callback_list_sequence(unsuccessful_sequence_6)

successful_sequence_6_1 = [
    lambda callback: engine.show_dialogue("I love jokes!", callback = callback),
    lambda callback: engine.show_dialogue("'"+engine.get_terminal_text(1)+"' ..." , callback = callback),
    lambda callback: engine.show_dialogue("Really...? That's the lamest joke in all of Pyland!", callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]

successful_sequence_6_2 = [
    lambda callback: engine.show_dialogue("I love me a good joke!", callback = callback),
    lambda callback: engine.show_dialogue("'"+engine.get_terminal_text(1)+"'. Hahahaha!" , callback = callback),
    lambda callback: engine.show_dialogue("Mate you're hilarious.", callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]

successful_sequence_6_3 = [
    lambda callback: engine.show_dialogue("Life is always better with a good joke!", callback = callback),
    lambda callback: engine.show_dialogue("'"+engine.get_terminal_text(1)+"'. Huh?" , callback = callback),
    lambda callback: engine.show_dialogue("Not sure I get the joke but thanks for trying to lighten my day!", callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]


unsuccessful_sequence_6 = [
    lambda callback: myla.wait(1.5, callback = callback),
    lambda callback: myla.turn_to_face(player, callback = callback),
    lambda callback: engine.show_dialogue("Myla: Make sure the script runs and you use 'print('Your Joke')' too!", callback = callback),
    lambda callback: get_snake(6).start_turning(time = 0.1, frequency = 8, callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]

try_script_sequence_6 = [
    lambda callback: snake_run_script(6, run_callback = snake_finish_script_6),
]

cancel_script_sequence_6 = [
    lambda callback: get_snake(5).start_turning(time = 0.1, frequency = 8, callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]


complete_sequence_6 = [
    lambda callback: player.set_busy(True, callback = callback),
    lambda callback: get_snake(6).turn_to_face(player, callback = callback),
    lambda callback: engine.show_dialogue("Jokes give me life!", callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
]

try_to_leave_sequence = [
    lambda callback: player.set_busy(True, callback = callback),
    lambda callback: engine.show_dialogue("Myla: Don't leave "+engine.get_player_name()+", they need our help!", callback = callback),
    lambda callback: player.set_busy(False, callback = callback),
    lambda callback: player.move_east(callback = callback),
    #lambda callback: myla.follow(player, callback = callback)
]

myla_sequence = [
    lambda callback: engine.show_dialogue("Lets help out all of the nervous snakes!", callback = callback),
]


def follow_all():
    for snake_index in range(len(snakes)-1):
        snakes[snake_index].follow(player)

finish_sequence = [
    lambda callback: elisabeth.set_solidity(False, callback = callback),
    lambda callback: jamie.face_east(callback = callback),
    lambda callback: camera.move_to(player.get_position(), time = 0.0, callback = callback),
    lambda callback: camera.focus(callback = callback),
    lambda callback: jamie.turn_to_face(player, callback = callback),
    lambda callback: camera.move_to(jamie.get_position(), time = 1.25, callback = callback),
    lambda callback: engine.show_dialogue("Jamie: Looks like all our scripts are ready. What's happened to our minister?!", callback = callback),
    lambda callback: camera.move_to(alexandra.get_position(), time = 1.25, callback = callback),
    lambda callback: alexandra.turn_to_face(player, callback = callback),
    lambda callback: engine.show_dialogue("Alexandra: He's not come back! They must have taken him!!!", callback = callback),
    lambda callback: myla.stop_follow(callback = callback),
    lambda callback: myla.turn_to_face(player, callback = callback),
    lambda callback: camera.move_to(player.get_position(), time = 1.5, callback = callback),
    lambda callback: player.focus(callback = callback),
    lambda callback: engine.show_dialogue("Myla: I've not heard any noises outside for a while. We'll have to try and escape now.", callback = callback),
    lambda callback: jamie.move_west(callback = callback),
    lambda callback: engine.show_dialogue("Jamie: Lets do it. I trust in you both.", callback = callback),
    lambda callback: myla.follow(player, callback = callback),
    lambda callback: follow_all(),
]


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

snakes[0].player_action  = lambda player_object: snake_action(0)
snakes[1].player_action  = lambda player_object: snake_action(1)
snakes[2].player_action  = lambda player_object: snake_action(2)
snakes[3].player_action  = lambda player_object: snake_action(3)
snakes[4].player_action  = lambda player_object: snake_action(4)
snakes[5].player_action  = lambda player_object: snake_action(5)
snakes[6].player_action  = lambda player_object: snake_action(6)

myla.player_action = lambda player_object: engine.run_callback_list_sequence(myla_sequence)

engine.run_callback_list_sequence(dialogue_sequence)


def try_leave():
    if level_finished():
        player_data.complete_level_and_save()
        player_data.save_and_exit("/world_1")
    else:
        engine.run_callback_list_sequence(try_to_leave_sequence)

leave_hall.player_walked_on = lambda player: try_leave()#lambda player_object: engine.run_callback_list_sequence(try_to_leave_sequence)

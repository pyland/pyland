player_one.focus()

engine.play_music("eerie")
engine.set_ui_colours((200,255,200),(215,255,215)) #TODO: save these colours in the config.
engine.set_py_tabs(9)

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

sign_one.set_message("Stuck in the maze? MWAHAHA. I'm gonna give you a small hint though - Go 25 steps east")

sign_two.set_message("MWAHAHA. You will never get past these security guards!")

def helper_one_action(player_object):
    if(player_object.is_facing_south()):
        helper_one.face_north()
    elif(player_object.is_facing_east()):
        helper_one.face_west()
    elif(player_object.is_facing_north()):
        helper_one.face_south()
    else:
        helper_one.face_east()
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("I have been stuck here for ages!", callback = callback),
        lambda callback: engine.show_dialogue("Wish I had a device to help me solve problems!", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback),
    ])

helper_one.player_action = helper_one_action

puzzle_one = False
security_one.face_south()
def security_one_action(player_object):
    if(not puzzle_one):
        engine.run_callback_list_sequence([
            lambda callback: player_one.set_busy(True, callback = callback),
            lambda callback: engine.show_dialogue("I am security guard Alpha! I can't let you pass. I need to get back to my work...", callback = callback),
            lambda callback: engine.show_dialogue("I need to know the total cost of building 242 snake cages in pounds (£).", callback = callback),
            lambda callback: engine.show_dialogue("Each cage costs £17.", callback = callback),
            lambda callback: player_one.set_busy(False, callback = callback),
        ])
security_one.player_action = security_one_action

puzzle_two = False
security_two.face_west()
def security_two_action(player_object):
    if(not puzzle_one):
        engine.run_callback_list_sequence([
            lambda callback: player_one.set_busy(True, callback = callback),
           lambda callback: engine.show_dialogue("I am security guard Bravo! I can't let you pass. I need to get back to my work...", callback = callback),
            lambda callback: engine.show_dialogue("I need to know when how many minutes we have left before reporting to our boss.", callback = callback),
            lambda callback: engine.show_dialogue("Forty minutes ago, he said come in two hours.", callback = callback),
            lambda callback: player_one.set_busy(False, callback = callback),
        ])
security_two.player_action = security_two_action

puzzle_three = False
security_three.face_north()
def security_three_action(player_object):
    if(not puzzle_one):
        engine.run_callback_list_sequence([
            lambda callback: player_one.set_busy(True, callback = callback),
            lambda callback: engine.show_dialogue("I am security guard Charlie! I can't let you pass. I need to get back to my work...", callback = callback),
            lambda callback: engine.show_dialogue("I need to know when how many minutes we have left before reporting to our boss.", callback = callback),
            lambda callback: engine.show_dialogue("Forty minutes ago, he said come in two hours.", callback = callback),
            lambda callback: player_one.set_busy(False, callback = callback),
        ])
security_three.player_action = security_three_action

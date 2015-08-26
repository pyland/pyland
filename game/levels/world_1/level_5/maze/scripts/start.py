import random

player_one.focus()
myla.follow(player_one)

engine.play_music("heroic")
engine.set_ui_colours((200,255,200),(215,255,215)) #TODO: save these colours in the config.
engine.set_py_tabs(2)

def no_thanks(player_object):
    engine.run_callback_list_sequence([
        lambda callback: engine.show_dialogue("Myla: All right then!", callback = callback),
        lambda callback: player_object.set_busy(False)
    ])
17
def for_help(player_object):
    engine.run_callback_list_sequence([
        lambda callback: engine.clear_scripter(callback = callback),
        lambda callback: engine.insert_to_scripter("#Code to move 25 times to the east using a loop", callback = callback),
        lambda callback: engine.insert_to_scripter("\nfor i in range(25):", callback = callback),
        lambda callback: engine.insert_to_scripter("\n\tmove_east()", callback = callback),
        lambda callback: player_object.set_busy(False)
    ])

def help_with_for(player_object):
    engine.run_callback_list_sequence([
        lambda callback: player_object.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("Evil Guy: MWAHAHA! This maze will be the death of you!", callback = callback),
        lambda callback: engine.show_dialogue("Evil Guy: I'll give you a hint though, you need to go east by 25 steps", callback = callback),
        lambda callback: myla.start_animating(speed = 0.1, loop = False, forward = False, callback = callback)
    ], callback = lambda: engine.show_dialogue_with_options(
        "Myla: Do you want me to help you with that?",
        {
            "Yes": lambda: for_help(player_object),
            "No": lambda: no_thanks(player_object)
        }))

def arithmetic_help(player_object):
    engine.run_callback_list_sequence([
        lambda callback: engine.clear_scripter(callback = callback),
        lambda callback: engine.insert_to_scripter("#The symbol * is used to multiply numbers\n", callback = callback),
        lambda callback: engine.insert_to_scripter("#The symbol + is used to add numbers\n", callback = callback),
        lambda callback: engine.insert_to_scripter("#The symbol - is used to subtract one number from another\n", callback = callback),
        lambda callback: engine.insert_to_scripter("#The symbol / is used to divide one number by another\n", callback = callback),
        lambda callback: engine.insert_to_scripter("#Try and help the guard solve the problem by printing out an answer for him using \nprint()\n", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback),
        lambda callback: player_object.set_busy(False)
    ])

def help_with_arithmetic(player_object):
    engine.run_callback_list_sequence([
        lambda callback: player_object.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("Evil Guy: MWAHAHA! You will never get past these security guards!", callback = callback),
        lambda callback: engine.show_dialogue("Evil Guy: I have given them a bunch of problems they will be stuck on forever!", callback = callback),
        lambda callback: myla.start_animating(speed = 0.1, loop = False, forward = False, callback = callback)
    ], callback = lambda: engine.show_dialogue_with_options(
        "Myla: Do you want me to help you with that?",
        {
            "Yes": lambda: arithmetic_help(player_object),
            "No": lambda: no_thanks(player_object)
        }))

###
### Random helper dude
###

sign_one.player_action = help_with_for
sign_two.player_action = help_with_arithmetic

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
        lambda callback: engine.show_dialogue("Dug: I have been stuck here for ages!", callback = callback),
        lambda callback: engine.show_dialogue("Dug: Wish I had a device to help me solve problems!", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback),
    ])

helper_one.player_action = helper_one_action

def no_help(name):
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue(name + ": That's fair enough, mate.", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback)
    ])

def cancel_script():
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(False, callback = callback)
    ])

def puzzle_wrong(name):
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue(name + ": What's that? That doesn't seem to be right!", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback),
    ])

def solved_puzzle(name):
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue(name + ": Thanks chum!", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback),
    ])

def being_smart():
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("Myla: Oooh that's smart! Using the scripter to move them instead of helping them. Good idea!", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback),
    ])

"""
*** *** *** Start security one
"""
puzzle_one_solved = False
security_one.face_south()
security_one_org_posn = security_one.get_position()

puzzle_one_a = random.randint(11,25)
puzzle_one_b = random.randint(190, 430)

def security_one_action(player_object):
    global puzzle_one_solved
    security_one.turn_to_face(player_object)
    if(not puzzle_one_solved):
        engine.run_callback_list_sequence([
            lambda callback: player_one.set_busy(True, callback = callback),
            lambda callback: engine.show_dialogue("Alpha: I am security guard Alpha! I will let you pass if you help me.", callback = callback),
            lambda callback: engine.show_dialogue("Alpha: I need to know the total cost of building my snake cages in pounds", callback = callback)
        ],
        callback = lambda: engine.show_dialogue_with_options(
            "Alpha: Can you help me?",
            options = {
                "Yes": lambda: security_one_problem(),
                "No" : lambda: no_help("Alpha")
            })
        )
            
    else:
        solved_puzzle("Alpha")

def security_one_problem():
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("Thank you! Help me run a script!",  callback = callback),
        lambda callback: engine.show_external_script(
            confirm_callback = lambda: engine.run_callback_list_sequence(try_security_one),
            cancel_callback = lambda: cancel_script(),
            external_dialogue = "I need to know the cost of building " + str(puzzle_one_a) + " cages, each costing £" + str(puzzle_one_b) + " (in pounds)",
            script_init = lambda: engine.insert_to_scripter("print(1 + 2)")
        )
    ])

try_security_one = [
    lambda callback: engine.show_dialogue("Thank you! I'll try running it now.", callback = callback),
    lambda callback: player_one.set_busy(False, callback = callback),
    lambda callback: security_one.run_script(script_to_run = 10, callback = lambda: check_security_one())
]

def check_security_one():
    global puzzle_one_solved
    global security_one_org_posn

    #we are using 1 and not 0 because the last thing to be printed is something like "security_one's script has ended"
    if(engine.get_terminal_text(1) == str(puzzle_one_a * puzzle_one_b)):
        puzzle_one_solved = True
        puzzle_one_right()
    else:
        puzzle_wrong("Alpha")

    if(security_one.get_position() != security_one_org_posn):
        security_one_org_posn = security_one.get_position()
        being_smart()

def puzzle_one_right():
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("What's that? Oh Thank You! Let me go buy the cages now!", callback = callback),
        lambda callback: engine.show_dialogue("To thank you, I will let you pass!", callback = callback),
        lambda callback: security_one.move_north(callback = callback),
        lambda callback: security_one.move_east(callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback),
    ])

security_one.player_action = security_one_action


"""
*** *** *** Start security two
"""
puzzle_two_solved = False
security_two.face_north()
security_two_org_posn = security_two.get_position()

puzzle_two_a = random.randint(1,59)
puzzle_two_b = random.randint(2,10)

def security_two_action(player_object):
    global puzzle_two_solved
    security_two.turn_to_face(player_object)
    if(not puzzle_two_solved):
        engine.run_callback_list_sequence([
            lambda callback: player_one.set_busy(True, callback = callback),
            lambda callback: engine.show_dialogue("Bravo: I am security guard Bravo! I will let you pass if you help me.", callback = callback),
            lambda callback: engine.show_dialogue("Bravo: I need to know how many minutes I have left before meeting my bosses", callback = callback)
        ],
        callback = lambda: engine.show_dialogue_with_options(
            "Bravo: Can you help me?",
            options = {
                "Yes": lambda: security_two_problem(),
                "No" : lambda: no_help("Bravo")
            })
        )
            
    else:
        solved_puzzle("Bravo")

def security_two_problem():
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("Thank you! Help me run a script!",  callback = callback),
        lambda callback: engine.show_external_script(
            confirm_callback = lambda: engine.run_callback_list_sequence(try_security_two),
            cancel_callback = lambda: cancel_script(),
            external_dialogue = str(puzzle_two_a) + " minutes ago my bosses said come in " + str(puzzle_two_b) + " hours. How minutes do I have left until I meet them?",
            script_init = lambda: engine.insert_to_scripter("print(1 + 2)")
        )
    ])

try_security_two = [
    lambda callback: engine.show_dialogue("Thank you! I'll try running it now.", callback = callback),
    lambda callback: player_one.set_busy(False, callback = callback),
    lambda callback: security_two.run_script(script_to_run = 10, callback = lambda: check_security_two())
]

def check_security_two():
    global puzzle_two_solved
    global security_two_org_posn

    #we are using 1 and not 0 because the last thing to be printed is something like "security_one's script has ended"
    if(engine.get_terminal_text(1) == str(puzzle_two_b * 60 - puzzle_two_a)):
        puzzle_two_solved = True
        puzzle_two_right()
    else:
        puzzle_wrong("Bravo")

    if(security_two.get_position() != security_two_org_posn):
        security_two_org_posn = security_two.get_position()
        being_smart()

def puzzle_two_right():
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("What's that? Oh Thank You! Let me wait till then", callback = callback),
        lambda callback: engine.show_dialogue("To thank you, I will let you pass!", callback = callback),
        lambda callback: security_two.move_south(callback = callback),
        lambda callback: security_two.move_west(callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback),
    ])

security_two.player_action = security_two_action

"""
*** *** *** Start security three
"""
puzzle_three_solved = False
security_three.face_west()

def security_three_action(player_object):
    global puzzle_three_solved

    if(not puzzle_three_solved):
        engine.run_callback_list_sequence([
            lambda callback: player_one.set_busy(True, callback = callback),
            lambda callback: engine.show_dialogue("Charlie: I am security guard Charlie! I will let you pass if you help me.", callback = callback),
            lambda callback: engine.show_dialogue("Charlie: I need to find the 2 keys that I lost around here", callback = callback),
            lambda callback: engine.show_dialogue("Charlie: Can you find them and give them to me?", callback = callback),
            lambda callback: check_security_three(player_object)
        ])
    else:
        solved_puzzle("Charlie")


def check_security_three(player_object):
    if player_object.get_keys() >= 2:
        puzzle_three_solved = True
        player_object.add_keys(-2)
        engine.run_callback_list_sequence([
            lambda callback: player_one.set_busy(True, callback = callback),
            lambda callback: engine.show_dialogue("Charlie: What's that? You have two keys? Oh Thank You! ", callback = callback),
            lambda callback: engine.show_dialogue("Charlie: To thank you, I will let you pass!", callback = callback),
            lambda callback: security_three.move_east(callback = callback),
            lambda callback: security_three.move_north(callback = callback),
            lambda callback: player_one.set_busy(False, callback = callback)
        ])
    else:
        engine.run_callback_list_sequence([
            lambda callback: player_one.set_busy(True, callback = callback),
            lambda callback: engine.show_dialogue("Charlie: You have don't have 2 keys? Oh never mind.", callback = callback),
            lambda callback: player_one.set_busy(False, callback = callback)
        ])

security_three.player_action = security_three_action
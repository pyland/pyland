from script_state_container import ScriptStateContainer

player_one.focus()
myla.follow(player_one)

engine.play_music("eery")
engine.set_ui_colours((200,255,200),(215,255,215)) #TODO: save these colours in the config.
engine.set_py_tabs(9)

def no_thanks(player_object):
    engine.run_callback_list_sequence([
        lambda callback: engine.show_dialogue("All right then!", callback = callback),
        lambda callback: player_object.set_busy(False)
    ])

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
        lambda callback: engine.show_dialogue("MWAHAHA! This maze will be the death of you!", callback = callback),
        lambda callback: engine.show_dialogue("I'll give you a hint though, you need to go east by 25 steps", callback = callback),
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
        lambda callback: engine.show_dialogue("MWAHAHA! You will never get past these security guards!", callback = callback),
        lambda callback: engine.show_dialogue("I have given them a bunch of problems they will be stuck on forever!", callback = callback),
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
        lambda callback: engine.show_dialogue("I have been stuck here for ages!", callback = callback),
        lambda callback: engine.show_dialogue("Wish I had a device to help me solve problems!", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback),
    ])

helper_one.player_action = helper_one_action

def no_help():
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("That's fair enough, mate.", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback)
    ])

def cancel_script():
    engine.run_callback_list_sequence([
        lambda callback: player.set_busy(False, callback = callback)
    ])

"""
*** *** *** Start security one
"""
puzzle_one_solved = False
security_one.face_south()

def security_one_action(player_object):
    global puzzle_one_solved
    security_one.turn_to_face(player_object)
    if(not puzzle_one_solved):
        engine.run_callback_list_sequence([
                lambda callback: player_one.set_busy(True, callback = callback),
                lambda callback: engine.show_dialogue("I am security guard Alpha! I will let you pass if you help me.", callback = callback),
                lambda callback: engine.show_dialogue("I need to know the total cost of building 242 snake cages in pounds (£).", callback = callback),
                lambda callback: engine.show_dialogue("Each cage costs £17.", callback = callback)
            ],
            callback = lambda: engine.show_dialogue_with_options(
                "Can you help me?",
                options = {
                    "Yes": lambda: security_one_problem(),
                    "No" : lambda: no_help()
                }
            )
        )
            
    else:
        engine.run_callback_list_sequence([
            lambda callback: player_one.set_busy(True, callback = callback),
            lambda callback: engine.show_dialogue("Thanks chum!", callback = callback),
            lambda callback: player_one.set_busy(False, callback = callback),
        ])

def security_one_problem():
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("Thank you! Give me a command to execute!",  callback = callback),
        lambda callback: engine.show_external_script(
            confirm_callback = lambda: engine.run_callback_list_sequence(try_security_one),
            cancel_callback = lambda: cancel_script(),
            external_dialogue = "I need to know the cost of building 242 cages, each costing £17 (in pounds)",
            script_init = lambda: engine.insert_to_scripter("print(\"\")")
        )
    ])

try_security_one = [
    lambda callback: engine.show_dialogue("Thank you, it means a lot! I'll try running it now.", callback = callback),
    lambda callback: player_one.set_busy(False, callback = callback),
    lambda callback: security_one.run_script(script_to_run = 10, callback = lambda: check_security_one)
]

def check_security_one():
    pass

def puzzle_one_right():
    engine.run_callback_list_sequence([
            lambda callback: player_one.set_busy(True, callback = callback),
            lambda callback: engine.show_dialogue("What's that? Oh Thank You! Let me go buy the cages now!", callback = callback),
            lambda callback: engine.show_dialogue("To thank you, I will let you pass!", callback = callback),
            lambda callback: security_one.move_east(callback = callback),
            lambda callback: engine.clear_scripter(callback = callback),
            lambda callback: engine.insert_to_scripter(player_script, callback = callback),
            lambda callback: player_one.set_busy(False, callback = callback),
    ])

def puzzle_one_wrong():
    engine.run_callback_list_sequence([
            lambda callback: player_one.set_busy(True, callback = callback),
            lambda callback: engine.show_dialogue("What's that? Why are you telling me random stuff?", callback = callback),
            lambda callback: player_one.set_busy(False, callback = callback),
    ])

security_one.player_action = security_one_action

"""
lambda callback: engine.insert_to_scripter("#The symbol * is used to multiply numbers\n", callback = callback),
            lambda callback: engine.insert_to_scripter("#The symbol + is used to add numbers\n", callback = callback),
            lambda callback: engine.insert_to_scripter("#The symbol - is used to subtract one number from another\n", callback = callback),
            lambda callback: engine.insert_to_scripter("#The symbol / is used to divide one number by another\n", callback = callback),
            lambda callback: engine.insert_to_scripter("#Try and help the guard solve the problem by printing out an answer to him\nprint()\n", callback = callback),
            lambda callback: player_one.set_busy(False, callback = callback),
       ], get_puzzle_one(callback_correct = puzzle_one_right, callback_wrong = puzzle_one_wrong))
"""

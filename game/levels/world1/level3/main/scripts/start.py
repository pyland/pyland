#commence save-data set-up
world_name = "world1"
level_name = "level3"
map_name = "main"

engine.update_world_text("1")
engine.update_level_text("3")

player_data.load(engine.get_player_name())
player_data.set_map(world_name, level_name = level_name, map_name = map_name) #change the map and save that the map has changed
#end save-data set-up

def myla_introduces_programming_action(player_object):
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: player_one.face_west(callback = callback),
        lambda callback: engine.show_dialogue("The next thing I'm going to teach you is comments.", callback = callback),
        lambda callback: engine.show_dialogue("When programming, even with years of experience, you can sometimes find things difficult to understand.", callback = callback),
        lambda callback: engine.show_dialogue("Comments allow you to write explenations for what your program does without interfering with the program itself.", callback = callback),
        lambda callback: engine.show_external_script(
            external_dialogue = "Look at the program I just gave you, give it a good read and do with it what you want! If you can't see everything, use the '+' and '-' buttons to zoom in and out.",
            script_init = lambda: engine.run_callback_list_sequence([
                lambda callback: engine.clear_scripter(callback = callback),
                lambda callback: engine.insert_to_scripter("#Everything after the \"#\" symbol on the same line will be ignored.\n", callback = callback),
                lambda callback: engine.insert_to_scripter("#So you can write whatever you want here without your program being affected!\n", callback = callback),
                lambda callback: engine.insert_to_scripter('""" If you want to put a comment on multiple lines,\n', callback = callback),
                lambda callback: engine.insert_to_scripter('You can really easily do that by enclosing it between two sets of three quotes.\n', callback = callback),
                lambda callback: engine.insert_to_scripter('"""\n', callback = callback),
                lambda callback: engine.insert_to_scripter("#Try and write comments in your programs, it's good practise!\n", callback = callback),
                lambda callback: engine.insert_to_scripter("hello = \"Hello \" # Aha, here we are using variables again!\n", callback = callback),
                lambda callback: engine.insert_to_scripter("myname = \"Myla\"  # Just so you know, everything in between a set of quotes is called a string.\n", callback = callback),
                lambda callback: engine.insert_to_scripter("yourname = '" + engine.get_player_name() + "' # You can use single or double quotes!\n", callback = callback),
                lambda callback: engine.insert_to_scripter('""" Strings are how we manipulate text in python,\n', callback = callback),
                lambda callback: engine.insert_to_scripter('You can add strings together, give this to me to run to see what is printed!\n', callback = callback),
                lambda callback: engine.insert_to_scripter('"""\n', callback = callback),
                lambda callback: engine.insert_to_scripter("print(hello + myname + \"!\")\n", callback = callback), #TODO, add all the reactions to what the player can give 
                lambda callback: engine.insert_to_scripter("print(hello + yourname + \"!\")", callback = callback), #TODO, add all the reactions to what the player can give M
            ]),
            confirm_callback = lambda: engine.run_callback_list_sequence([
                lambda callback: engine.show_dialogue("Awesome!", callback = callback), #TODO: add all the possible reactions to what the player can give Myla
            ], callback = callback),
            cancel_callback = lambda: engine.run_callback_list_sequence([
                lambda callback: engine.show_dialogue("Oh, you've forgotten my name already? I'm Myla! Try giving it another go.", callback = callback),
            ], callback = callback),
        ),
        lambda callback: engine.show_dialogue("print() is an example of something called a function.", callback = callback),
        lambda callback: engine.show_dialogue("A function takes what you give it in between the brackets, and does something with it!", callback = callback),
        lambda callback: engine.show_dialogue("The thing you give to a function is called an argument.", callback = callback),
        lambda callback: engine.show_dialogue("If it's a lot to take in, don't worry, we will back to all of this again and again in many different ways.", callback = callback),
        lambda callback: engine.show_dialogue("In summary, you just used a function called print, and gave it my name as an argument.", callback = callback),
        lambda callback: engine.show_dialogue("Anyway, let's get moving, I will teach you some more in a bit.", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback),
    ])

myla_introduces_programming.player_walked_on = myla_introduces_programming_action

def myla_introduces_variables_action(player_object):
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: player_one.face_east(callback = callback),
        lambda callback: engine.show_dialogue("Now I'm going to teach you how to write your own function.", callback = callback),
        lambda callback: engine.show_external_script(
            external_dialogue = "Try printing your name this time! However, do it by changing what name is equals to, don't give it straight to print!",
            script_init = lambda: engine.run_callback_list_sequence([
                lambda callback: engine.clear_scripter(callback = callback),
                lambda callback: engine.insert_to_scripter("name = \"Myla\"\n", callback = callback),
                lambda callback: engine.insert_to_scripter("print(name)", callback = callback), #TODO, add all the reactions to what the player can give Myla
            ]),
            confirm_callback = lambda: engine.run_callback_list_sequence([
                lambda callback: engine.show_dialogue("Awesome!", callback = callback),
            ], callback = callback),
            cancel_callback = lambda: engine.run_callback_list_sequence([
                lambda callback: engine.show_dialogue("Oh, you've forgotten your own name already? Hahaha, you're called " + engine.get_player_name() + " silly.", callback = callback),
            ], callback = callback)
        ),
        lambda callback: engine.show_dialogue("Well done " + engine.get_player_name() + "!", callback = callback),
        lambda callback: engine.show_dialogue("Variables are really useful for making your program remember things, they will really come in handy for when we do maths.", callback = callback),
        lambda callback: engine.show_dialogue("Don't worry if you don't like doing maths much, the thing that's useful about programming is that the PyScripter does all the maths for you!", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback),
    ])

myla_introduces_variables.player_walked_on = myla_introduces_variables_action


#setting the player's starting position
engine.run_callback_list_sequence([
    lambda callback: player_one.move_to(exit_to_world.get_position(), callback = callback),
    lambda callback: player_one.move_north(callback = callback),
])
myla_start_position = exit_to_world.get_position()
#end setting the player's starting position





def go_to_world(player_object):
    player_data.save_and_exit("/world1")

def go_to_bogs(player_object):
    player_data.complete_level_and_save()
    player_data.save_and_exit("/world1")

exit_to_world.player_walked_on = go_to_world
exit_to_bogs.player_walked_on = go_to_bogs

myla.set_solidity(False)
myla.move_to(myla_start_position)
myla.follow(player_one)

player_one.focus()
engine.play_music("world_1_jungle")

toll_person.set_dialogue_list([
    "This person will want to toll you to cross the bridge, before letting you through",
    "However, their PyRunner doesn't work and Myla cheekily tells you to 'fix' it so that...",
    "...it always show that everyone has payed.",
    "That will resolve and complete the level. The script the toll_person has will be to the effect of...",
    "amount_payed = \"£0\" \n print(amount_payed)",
    "The played can replace the amount_payed with whatever they want above the charge of £10.",
    "That will complete the level",
])


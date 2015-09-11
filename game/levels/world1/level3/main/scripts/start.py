#commence save-data set-up
world_name = "world1"
level_name = "level3"
map_name = "main"

engine.update_world_text("1")
engine.update_level_text("3")

player_data.load(engine.get_player_name())
player_data.set_map(world_name, level_name = level_name, map_name = map_name) #change the map and save that the map has changed
#end save-data set-up

def myla_introduces_comments_action(player_object):
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: player_one.face_west(callback = callback),
        lambda callback: engine.show_dialogue("The next thing I'm going to teach you is comments.", callback = callback),
        lambda callback: engine.show_dialogue("When programming, even with years of experience, you can sometimes find things difficult to understand.", callback = callback),
        lambda callback: engine.show_dialogue("Comments allow you to write explenations for what your program does without interfering with the program itself.", callback = callback),
        lambda callback: engine.show_external_script(
            external_dialogue = "Look at the program I just gave you, give it a good read and try running it! If you can't see everything, use the '+' and '-' buttons in the bottom-left hand corner to zoom in and out.",
            script_init = lambda: engine.run_callback_list_sequence([
                lambda callback: engine.clear_scripter(callback = callback),
                lambda callback: engine.insert_to_scripter("#Everything after the \"#\" symbol on the same line will be ignored.\n", callback = callback),
                lambda callback: engine.insert_to_scripter("#So you can write whatever you want here without your program being affected!\n", callback = callback),
                lambda callback: engine.insert_to_scripter("#Try and write comments in your programs, it's good practise!\n", callback = callback),
                lambda callback: engine.insert_to_scripter("hello = \"Hello \" # Aha, here we are using variables again!\n", callback = callback),
                lambda callback: engine.insert_to_scripter("myname = \"Myla\"  # Just so you know, all text you use had to be in quotes.\n", callback = callback),
                lambda callback: engine.insert_to_scripter("yourname = '" + engine.get_player_name() + "' # You can use single or double quotes!\n", callback = callback),
                lambda callback: engine.insert_to_scripter('# A piece of text in a programm is called a string.\n', callback = callback),
                lambda callback: engine.insert_to_scripter('# You can add strings together, give this to me to run to see what is printed!\n', callback = callback),
                lambda callback: engine.insert_to_scripter("print(hello + myname + \"!\")\n", callback = callback), #TODO, add all the reactions to what the player can give 
                lambda callback: engine.insert_to_scripter("#print(hello + yourname + \"!\") #Remove the hash form the start of this line to print: Hello " + engine.get_player_name() + "!", callback = callback), #TODO, add all the reactions to what the player can give Myla
            ]),
            confirm_callback = lambda: engine.run_callback_list_sequence([
                lambda callback: engine.show_dialogue("Awesome! I know that was a lot to read.", callback = callback), #TODO: add all the possible reactions to what the player can give Myla
            ], callback = callback),
            cancel_callback = lambda: engine.run_callback_list_sequence([
                lambda callback: engine.show_dialogue("I would at least give it to me to run! TODO: go back to running", callback = callback),
            ], callback = callback),
        ),
        lambda callback: player_one.set_busy(False, callback = callback),
    ])

myla_introduces_comments.player_walked_on = myla_introduces_comments_action

def myla_introduces_functions_action(player_object):
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: player_one.face_south(callback = callback),
        lambda callback: engine.show_dialogue("Now I'm going to teach you how to write your own function.", callback = callback),
        lambda callback: engine.show_dialogue("You have already used functions before, 'print' was one.", callback = callback),
        lambda callback: engine.show_dialogue("A function is simply a way of giving some code a name, so that you can use it again and again.", callback = callback),
        lambda callback: engine.show_external_script(
            external_dialogue = "Here is an example function, don't forget to read the comments! We will go over how to write them later, just get used to what they look like and changing them.",
            script_init = lambda: engine.run_callback_list_sequence([
                lambda callback: engine.clear_scripter(callback = callback),
                lambda callback: engine.insert_to_scripter("#This is a function I made called print_hello, however...\n", callback = callback),
                lambda callback: engine.insert_to_scripter("#It prints 'Hi Myla?', can you change it so that it prints 'Hello Myla!'?\n", callback = callback),
                lambda callback: engine.insert_to_scripter("def print_hello(name):\n", callback = callback),
                lambda callback: engine.insert_to_scripter("  #To tell python when your function has finished. \n", callback = callback),
                lambda callback: engine.insert_to_scripter("  #We 'indent' all code in the function with the same amount of spaces.\n", callback = callback),
                lambda callback: engine.insert_to_scripter("  #So everything in this function has two spaces in front of it\n", callback = callback),
                lambda callback: engine.insert_to_scripter("  print(\"Hi \" + name + \"?\") # Here you can see we are using 'name' as a variable again!\n", callback = callback),
                lambda callback: engine.insert_to_scripter("  #This comment is still part of the function!\n", callback = callback),
                lambda callback: engine.insert_to_scripter("#This comment has no spaces in front of it, meaning it's not part of the function.\n", callback = callback),
                lambda callback: engine.insert_to_scripter("print_hello('Myla') #This is using the function you made, you want it to print 'Hello Myla!'", callback = callback), #TODO, add all the reactions to what the player can give Myla
            ]),
            confirm_callback = lambda: engine.run_callback_list_sequence([
                lambda callback: engine.show_dialogue("Awesome!", callback = callback),
            ], callback = callback),
            cancel_callback = lambda: engine.run_callback_list_sequence([
                lambda callback: engine.show_dialogue("Here is the printing section.", callback = callback),
            ], callback = callback)
        ),
        lambda callback: engine.show_dialogue("Well done " + engine.get_player_name() + "!", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback),
    ])

myla_introduces_functions.player_walked_on = myla_introduces_functions_action

def card_person_action(player_object):
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: player_one.turn_to_face(card_person, callback = callback),
        lambda callback: engine.show_dialogue("My PyRunner broke. It was a really cool and I had bought an awesome script that let me make birthday cards automatically.", callback = callback),
        lambda callback: engine.show_dialogue("The storm broke it so now function I used doesn't work anymore.", callback = callback),
        lambda callback: player_one.face_west(callback = callback),
        lambda callback: engine.show_external_script(
            external_dialogue = "I want it to say 'Dear name, Happy Birthday!'.",
            script_init = lambda: engine.run_callback_list_sequence([
                lambda callback: engine.clear_scripter(callback = callback),
                lambda callback: engine.insert_to_scripter("def make_birthday_card(name):\n", callback = callback),
                lambda callback: engine.insert_to_scripter("  print('Dear 'name',')\n", callback = callback), #TODO, add all the reactions to what the player can give Myla
                lambda callback: engine.insert_to_scripter("  print('Merry Christmas!')\n", callback = callback), #TODO, add all the reactions to what the player can give Myla
            ]),
            confirm_callback = lambda: engine.run_callback_list_sequence([
                lambda callback: engine.show_dialogue("TODO: All the reactions!", callback = callback),
            ], callback = callback),
            cancel_callback = lambda: engine.run_callback_list_sequence([
                lambda callback: player_one.face_south(callback = callback),
                lambda callback: engine.show_dialogue("Myla, remember you need a '+' to add strings (pieces of text) together.", callback = callback),
            ], callback = callback)
        ),
        lambda callback: engine.show_dialogue("Thank you!", callback = callback),
        lambda callback: myla.follow(player_one, callback = callback),
        lambda callback: card_person.move_south(callback = callback),
        lambda callback: card_person.move_west(callback = callback),
        lambda callback: card_person.move_south(callback = callback),
        lambda callback: card_person.face_north(callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback),
    ])

card_person.player_action = card_person_action
    
teach_enter_to_talk.player_walked_on = lambda player_object: engine.run_callback_list_sequence([
    lambda callback: player_one.set_busy(True, callback = callback),
    lambda callback: engine.show_dialogue("You can press 'Enter' to talk to people.", callback = callback),
    lambda callback: player_one.set_busy(False, callback = callback),
])

#setting the player's starting position
engine.run_callback_list_sequence([
    lambda callback: player_one.move_to(exit_to_world.get_position(), callback = callback),
    lambda callback: player_one.move_west(callback = callback),
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

card_person.face_north()

croc.move_vertical()



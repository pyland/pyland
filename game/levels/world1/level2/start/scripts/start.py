#commence save-data set-up
world_name = "world1"
level_name = "level2"
map_name = "start"

engine.update_world_text("1")
engine.update_level_text("2")

player_data.load(engine.get_player_name())
player_data.set_map(world_name, level_name = level_name, map_name = map_name) #change the map and save that the map has changed
#end save-data set-up

def myla_introduces_programming_action(player_object):
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: player_one.face_west(callback = callback),
        lambda callback: engine.show_dialogue("Myla: This is as good place to start as any!", callback = callback),
        lambda callback: engine.show_dialogue("Just a quick question though, the PyScripter uses a programming language called Python.", callback = callback),
        lambda callback: engine.show_dialogue("Have you ever programmed in Python before? (TODO ASK QUESTION AND GIVE APPROPRIATE RESPONSE)", callback = callback),
        lambda callback: engine.show_dialogue("The first thing we are going to start with is printing to the the PyConsole in the bottom right-hand corner of the screen.", callback = callback),
        lambda callback: engine.show_dialogue("To start with, I am going to ask you to give the scripts for me to check and run.", callback = callback),
        lambda callback: engine.show_external_script(
            script_init = lambda: engine.run_callback_list_sequence([
                lambda callback: engine.clear_scripter(callback = callback),
                lambda callback: engine.insert_to_scripter("print()"),
            ]),
            confirm_callback = lambda: engine.run_callback_list_sequence([
                lambda callback: engine.show_dialogue("Awesome!", callback = callback),
            ], callback = callback),
            cancel_callback = lambda: engine.run_callback_list_sequence([
                lambda callback: engine.show_dialogue("Oh, you've forgotten my name already? I'm Myla! Try giving it another go.", callback = callback),
            ], callback = callback),
            external_dialogue = "Can you please print my name? Don't forget to put it in quotation marks!"
        ),
        lambda callback: player_one.set_busy(False, callback = callback),
    ])

myla_introduces_programming.player_walked_on = myla_introduces_programming_action


#setting the player's starting position
if player_data.previous_exit_is("world1", level_name = "level2", map_name = "bogs"):
    engine.run_callback_list_sequence([
        lambda callback: player_one.move_to(exit_to_bogs.get_position(), callback = callback),
        lambda callback: player_one.move_north(callback = callback),
    ])
    myla_start_position = exit_to_bogs.get_position()
elif player_data.previous_exit_is("world1"):
    player_one.move_east()
    myla_start_position = (3, 10)
#end setting the player's starting position





def go_to_world(player_object):
    player_data.save_and_exit("/world1")

def go_to_bogs(player_object):
    player_data.save_and_exit("/world1/level2/bogs")

exit_to_world.player_walked_on = go_to_world
exit_to_bogs.player_walked_on = go_to_bogs

myla.move_to(myla_start_position)
myla.follow(player_one)

player_one.focus()
engine.play_music("world_1_jungle")

bog_sign.set_message("(west) rossumberg (south) bog")

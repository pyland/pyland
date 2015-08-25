#commence save-data set-up
world_name = "world_1"
level_name = "level_2"
map_name = "bogs"

player_data.load(engine.get_player_name())
player_data.set_map(world_name, level_name = level_name, map_name = map_name) #change the map and save that the map has changed
#end save-data set-up

#setting the player's starting position
if player_data.previous_exit_is("world_1", level_name = "level_2", map_name = "start"):
    player_one.move_south()
    myla_start_position = exit_to_start.get_position()
    myla.set_solidity(False, callback = lambda: myla.move_to(myla_start_position, callback = lambda: myla.follow(player_one)))
#end setting the player's starting position

def go_to_start(player_object):
    player_data.save_and_exit("/world_1/level_2/start")

exit_to_start.player_walked_on = go_to_start


bog_sign.set_message("Welcome to the bogs! The home of stinky water...")

player_one.focus()
engine.play_music("world_1_jungle")
croc_guard_totem.face_north()

vine1.grow(player_one)

def myla_explain_cut_action(player_object):
    def myla_ask_player_if_there_is_way(callback):
        engine.show_dialogue_with_options(
            "Aren't you gonna ask me if there is a way?",
            {
                "Yes" : lambda: engine.show_dialogue("Why thank you, you can use your *ahem*, my PyScripter!", callback = callback),
                "No": lambda: engine.show_dialogue("Argh ok you win. You can use the PyScripter.", callback = callback)
            }
        )

    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: player_one.face_east(callback = callback),
        lambda callback: engine.show_dialogue(engine.get_player_name() + ", there's a vine in the way over there.", callback = callback),
        lambda callback: engine.show_dialogue("What's that, I hear you ask? How could we possibly make it through?", callback = callback),
        lambda callback: engine.show_dialogue("If only there was a way...", callback = callback),
        lambda callback: engine.show_dialogue("...", callback = callback),
        myla_ask_player_if_there_is_way,
        lambda callback: engine.show_dialogue("I'm going to give you my knife so that you can cut down vines.", callback = callback),
        lambda callback: engine.show_dialogue("The PyScripter let's you control objects from the real world. So you can use it to cut down vines!", callback = callback),
        lambda callback: engine.show_dialogue("I have written a small script for you and put it into your PyScripter...", callback = callback),
        lambda callback: engine.show_dialogue("Stand next to a vine and hit the 'Run' button (or you can use the spacebar to run scripts)!", callback = callback),
        lambda callback: engine.clear_scripter(callback = callback),
        lambda callback: engine.insert_to_scripter("cut()", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback)
    ])


myla_explain_cut.player_walked_on = myla_explain_cut_action

def myla_explain_crocodiles_action(player_object):
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("Here Myla would explain that there are sleeping crocodiles here.", callback = callback),
        lambda callback: engine.show_dialogue("She will also get in your bag to hide from them.", callback = callback),
        lambda callback: engine.show_dialogue("If you stand next to a sleeping crocodile it 'gets' you.", callback = callback),
        lambda callback: engine.show_dialogue("So you have to cut the vines to proceed.", callback = callback),
        lambda callback: engine.show_dialogue("Myla will get in the player's bag to hide from the crocs.", callback = callback),
        lambda callback: engine.show_dialogue("Ying's levels properly introduce player mechanics for interacting with crocs.", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback)
    ])

myla_explain_crocodiles.player_walked_on = myla_explain_crocodiles_action

def myla_explain_basic_move_action(player_object):
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("Here the basic move functions will be introduced and automatically inserted into the scripter.", callback = callback),
        lambda callback: engine.show_dialogue("It simply introduces the functionality to the player.", callback = callback),
        lambda callback: engine.clear_scripter(callback = callback),
        lambda callback: engine.insert_to_scripter("move_east()\nmove_south()\nmove_west()\nmove_north()", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback)
    ])

myla_explain_basic_move.player_walked_on = myla_explain_basic_move_action

def myla_give_simple_loop_action(player_object):
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("Here a simple loop to speed up movement is given to the player that they can run. To move east to the end.", callback = callback),
        lambda callback: engine.show_dialogue("The player will also be referred to the 'speed toggle' button.", callback = callback),
        lambda callback: engine.clear_scripter(callback = callback),
        lambda callback: engine.insert_to_scripter("while True:\n  move_east()", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback)
    ])

myla_give_simple_loop.player_walked_on = myla_give_simple_loop_action

def myla_tells_loop_modify_action(player_object):
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("Here Myla will ask the player to replace the move_east() with a move_south(), and run that to save time.", callback = callback),
        lambda callback: engine.clear_scripter(callback = callback),
        lambda callback: engine.insert_to_scripter("while True:\n  move_east()", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback)
    ])

myla_tells_loop_modify.player_walked_on = myla_tells_loop_modify_action

def myla_give_vine_loop_action(player_object):
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("There will be a lot of vines in the way up north. So Myla will give the player another script...", callback = callback),
        lambda callback: engine.clear_scripter(callback = callback),
        lambda callback: engine.insert_to_scripter("while True:\n  move_north()\n  cut()", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback)
    ])

myla_give_vine_loop.player_walked_on = myla_give_vine_loop_action

def myla_notice_bog_water_action(player_object):
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("Here the player will be able to get bog-water and proceed to the next level with Myla...", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback)
    ])

myla_notice_bog_water.player_walked_on = myla_notice_bog_water_action


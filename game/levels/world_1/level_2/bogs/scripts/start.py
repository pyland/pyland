#commence save-data set-up
world_name = "world_1"
level_name = "level_2"
map_name = "bogs"

player_data.load(engine.get_player_name())
#player_data.set_map(world_name, level_name = level_name, map_name = map_name) #change the map and save that the map has changed
#end save-data set-up

#setting the player's starting position
if player_data.previous_exit_is("world_1", level_name = "level_2", map_name = "start"):
    player_one.move_south()
#end setting the player's starting position

bog_sign.set_message("Welcome to the bogs! The home of stinky water...")

player_one.focus()
engine.play_music("world_1_jungle")
croc_guard_totem.face_north()

vine1.grow(player_one)

myla.follow(player_one)

def myla_explain_cut_action(player_object):
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("Hi however is reading this early!", callback = callback),
        lambda callback: engine.show_dialogue("This level is early and incomplete, so I will place messages where the level's events...", callback = callback),
        lambda callback: engine.show_dialogue("... would normally occur and explain what would happen so that you can give feedback on that.", callback = callback),
        lambda callback: engine.show_dialogue("Here Myla would give the cut script to the player, and show it's usefulness for cutting down vines.", callback = callback),
        lambda callback: engine.show_dialogue("I will also edit the earlier levels slightly with secrets hidden behind vines, that the player can...", callback = callback),
        lambda callback: engine.show_dialogue("... retroactively get.", callback = callback),
        lambda callback: engine.show_dialogue("Anindya's levels will expand on the usage of cut.", callback = callback),
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


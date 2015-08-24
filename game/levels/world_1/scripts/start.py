#commence save-data set-up
world_name = "world_1"

player_data.load(engine.get_player_name())
player_data.set_map(world_name) #change the map and save that the map has changed
#end save-data set-up


#setting the player's starting position
if player_data.previous_exit_is("world_1", level_name = "level_1"):
    player_one.move_to(level_1.get_position())
elif player_data.previous_exit_is("world_1", level_name = "level_2"):
    player_one.move_to(level_2.get_position())
elif player_data.previous_exit_is("world_1", level_name = "level_3"):
    player_one.move_to(level_3.get_position())
elif player_data.previous_exit_is("world_1", level_name = "level_4"):
    player_one.move_to(level_4.get_position())
elif player_data.previous_exit_is("world_1", level_name = "level_5"):
    player_one.move_to(level_5.get_position())
elif player_data.previous_exit_is("world_1", level_name = "level_6"):
    player_one.move_to(level_6.get_position())
elif player_data.previous_exit_is("world_1", level_name = "level_7"):
    player_one.move_to(level_7.get_position())
elif player_data.previous_exit_is("world_1", level_name = "level_8"):
    player_one.move_to(level_8.get_position())
#end setting the player's starting position

def ask_to_go_to_level(map_location, readable_name):
    player_data.save_and_exit(map_location)

def go_to_level_1(player_object):
    ask_to_go_to_level("/world_1/level_1/player_house", "Level 1")

level_1.player_walked_on = go_to_level_1

def go_to_level_2(player_object):
    ask_to_go_to_level("/world_1/level_2/start", "Level 2")

level_2.player_walked_on = go_to_level_2

def go_to_level_3(player_object):
    ask_to_go_to_level("/world_1/level_3/mainb", "Level 3")

level_3.player_walked_on = go_to_level_3

def go_to_level_4(player_object):
    ask_to_go_to_level("/world_1/level_4/main_b", "Level 4")

level_4.player_walked_on = go_to_level_4

def go_to_level_5(player_object):
    ask_to_go_to_level("/world_1/level_5/maze", "Level 5")

level_5.player_walked_on = go_to_level_5

def go_to_level_6(player_object):
    ask_to_go_to_level("/world_1/level_6/vine_intro", "Level 6")

level_6.player_walked_on = go_to_level_6

def go_to_level_7(player_object):
    ask_to_go_to_level("/world_1/level_7/village", "Level 7")

level_7.player_walked_on = go_to_level_7

def go_to_level_8(player_object):
    pass
    #ask_to_go_to_level("/world_1/level_8/start", "Level 8")

level_8.player_walked_on = go_to_level_8

player_one.focus()

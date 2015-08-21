#commence save-data set-up
world_name = "world_1"
current_map = "/" + world_name

player_name = engine.get_player_name()
player_data = engine.get_player_data(player_name)
player_data["current_map"] = current_map
level_data = player_data["level_data"][world_name]

engine.save_player_data(player_name, player_data)
#end save-data set-up

def go_to_level_1(player_object):
    player_data["previous_exit"] = current_map
    engine.save_player_data(player_name, player_data)
    engine.change_map("/world_1/level_1/player_house")

level_1.player_walked_on = go_to_level_1

def go_to_level_2(player_object):
    player_data["previous_exit"] = current_map
    engine.save_player_data(player_name, player_data)
    engine.change_map("/world_1/level_2/start")

level_2.player_walked_on = go_to_level_2

player_one.focus()

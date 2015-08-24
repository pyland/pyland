#commence save-data set-up
world_name = "world_1"

player_data.load(engine.get_player_name())
player_data.set_map(world_name) #change the map and save that the map has changed
#end save-data set-up

def go_to_level_1(player_object):
    player_data.save_and_exit("/world_1/level_1/player_house")

level_1.player_walked_on = go_to_level_1

def go_to_level_2(player_object):
    player_data.save_and_exit("/world_1/level_2/start")

level_2.player_walked_on = go_to_level_2

player_one.focus()

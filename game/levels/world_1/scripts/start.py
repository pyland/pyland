#TODO: Get all the initialisation working!!!!!!!!!!!!!!!!!!!!!!

def go_to_level_1(player_object):
    engine.change_map("/world_1/level_1/player_house")

level_1.player_walked_on = go_to_level_1

def go_to_level_2(player_object):
    engine.change_map("/world_1/level_2/start")

level_2.player_walked_on = go_to_level_2

player_one.focus()

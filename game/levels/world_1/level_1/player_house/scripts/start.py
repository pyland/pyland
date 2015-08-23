#commence save-data set-up
level_name = "level_1"
world_name = "world_1"
map_name = "player_house"
current_map = "/" + world_name + "/" + level_name + "/" + map_name

player_name = engine.get_player_name()
player_data = engine.get_player_data(player_name)
player_data["current_map"] = current_map
level_data = player_data["level_data"][world_name][level_name]

engine.save_player_data(player_name, player_data)
#end save-data set-up

#setting the player's starting position
if player_data["previous_exit"] == "/world_1/intro":
    pass  #start player in default 
elif player_data["previous_exit"] == "/world_1/level_1/rossumberg":
    player_one.move_to((3, 4), callback = player_one.move_south)

#end setting the player's starting position
    
player_one.focus()

engine.play_music("calm")

def go_to_village(player_object):
    player_data["previous_exit"] = current_map
    engine.save_player_data(player_name, player_data)
    engine.change_map("/world_1/level_1/rossumberg")

exit_to_village.player_walked_on = go_to_village

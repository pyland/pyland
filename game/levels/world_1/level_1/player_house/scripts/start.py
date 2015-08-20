#commence set-up
player_name = engine.get_player_name()
player_data = engine.get_player_data(player_name)
current_level = "/world_1/level_1/player_house" #TODO: make this an engine function!!!!

#setting the player starting position before anything else based on which level the player was at previously
if player_data["current_level"] == "/world_1/intro":
    pass
elif player_data["current_level"] == "/world_1/level_1/rossumberg":
    player_one.move_to((3, 4), callback = player_one.face_south)

player_data["current_level"] = current_level
# Add the basic level data to the save file if it doesn't exist
if not (current_level in player_data["level_states"]):
    player_data["level_states"][current_level] = {}

engine.save_player_data(player_name, player_data)

level_state = player_data["level_states"][current_level]
#end set-up
    


player_one.focus()

engine.play_music("calm")

def go_to_village(player_object):
    engine.change_map("/world_1/level_1/rossumberg")

exit_to_village.player_walked_on = go_to_village

#commence save-data set-up
level_name = "level_1"
world_name = "world_1"
map_name = "player_house"

engine.update_world_text("1")
engine.update_level_text("1")

engine.set_ui_colours((200,255,200),(215,255,215))

player_data.load(engine.get_player_name())
player_data.set_map(world_name, level_name = level_name, map_name = map_name) #change the map and save that the map has changed
#end save-data set-up

#setting the player's starting position
if player_data.get_previous_exit() == "/world_1/intro":
    pass  #start player in default
elif player_data.get_previous_exit() == "/world_1/level_1/rossumberg":
    player_one.move_to((3, 4), callback = player_one.move_south)

#end setting the player's starting position

player_one.focus()

engine.play_music("calm")

def go_to_village(player_object):
    player_data.save_and_exit("/world_1/level_1/rossumberg")

exit_to_village.player_walked_on = lambda player_one: engine.change_map("world_1/level_1/rossumberg")#go_to_village

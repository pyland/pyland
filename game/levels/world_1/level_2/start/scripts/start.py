#commence save-data set-up
world_name = "world_1"
level_name = "level_2"
map_name = "start"

player_data.load(engine.get_player_name())
player_data.set_map(world_name, level_name = level_name, map_name = map_name) #change the map and save that the map has changed
#end save-data set-up

def go_to_world(player_object):
    player_data.save_and_exit("/world_1")

exit_to_world.player_walked_on = go_to_world


def pyscripter_player_action(player_object):
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: pyscripter.set_visible(False, callback = callback),
        lambda callback: pyscripter.set_solidity(False, callback = callback),
        lambda callback: engine.show_dialogue(engine.get_player_name() + " picked up the PyScripter!", callback = callback),
        #lambda callback: engine.show_dialogue("Annoying Croc: Stop right there!", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback)
    ])

pyscripter.player_action = pyscripter_player_action

player_one.focus()

bog_sign.set_message("(west) rossumberg (south) bog")


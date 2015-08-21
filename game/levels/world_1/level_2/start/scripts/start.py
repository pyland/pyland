#commence save-data set-up
level_name = "level_2"
world_name = "world_1"
map_name = "start"
current_map = "/" + world_name + "/" + level_name + "/" + map_name

player_name = engine.get_player_name()
player_data = engine.get_player_data(player_name)
player_data["current_map"] = current_map
level_data = player_data["level_data"][world_name][level_name]

engine.save_player_data(player_name, player_data)
#end save-data set-up

def go_to_world(player_object):
    player_data["previous_exit"] = "/" + world_name + "/" + level_name
    engine.save_player_data(player_name, player_data)
    engine.change_map("/world_1")

exit_to_world.player_walked_on = go_to_world


def pyscripter_player_action(player_object):
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: pyscripter.set_visible(False, callback = callback),
        lambda callback: pyscripter.set_solidity(False, callback = callback),
        lambda callback: engine.show_dialogue(engine.get_player_name() + " picked up the ... ", callback = callback),
        #lambda callback: engine.show_dialogue(" ... ", callback = callback),
        #lambda callback: engine.show_dialogue(" ... ", callback = callback),
        #lambda callback: engine.show_dialogue("Annoying Croc: Stop right there!", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback)
    ])

pyscripter.player_action = pyscripter_player_action

player_one.focus()

bog_sign.set_message("(west) rossumberg (south) bog")


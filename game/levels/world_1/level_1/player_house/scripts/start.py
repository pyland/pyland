player_one.focus()

engine.play_music("calm")

def go_to_village(player_object):
    engine.change_map("/world_1/level_1/rossumberg")

exit_to_village.player_walked_on = go_to_village

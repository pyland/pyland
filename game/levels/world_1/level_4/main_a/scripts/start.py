#commence save-data set-up
world_name = "world_1"
level_name = "level_4"
map_name = "main_a"

player_data.load(engine.get_player_name())
player_data.set_map(world_name, level_name = level_name, map_name = map_name) #change the map and save that the map has changed
#end save-data set-up


player_one.focus()

croc_0.face_east()
croc_1.face_east()
croc_2.face_east()
croc_3.face_east()


engine.run_callback_list_sequence([
    lambda callback: player_one.face_east(callback = callback),
    lambda callback: player_one.set_busy(True, callback = callback),
    lambda callback: myla.face_west(callback = callback),
    lambda callback: engine.show_dialogue("Some of these crocodiles look different...", callback = callback),
    lambda callback: engine.show_dialogue("Let's try yelling at them again to see what they do.", callback =callback),
    lambda callback: myla.follow(player_one, callback = callback),
    lambda callback: player_one.set_busy(False, callback =callback)
    ])

def go_next(player_object):
    player_data.save_and_exit("/world_1/level_4/main_b")

end_level.player_walked_on = go_next
end_level_2.player_walked_on = go_next




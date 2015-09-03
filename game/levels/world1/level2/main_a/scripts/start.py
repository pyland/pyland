#commence save-data set-up
world_name = "world_1"
level_name = "level_2"
map_name = "main_a"

engine.update_world_text("1")
engine.update_level_text("2")

player_data.load(engine.get_player_name())
player_data.set_map(world_name, level_name = level_name, map_name = map_name) #change the map and save that the map has changed
#end save-data set-up

came_from_main_b = False

if player_data.previous_exit_is(world_name, level_name = level_name, map_name = "main_b"):
    x, y = end_level.get_position()
    came_from_main_b = True
    player_one.move_to((x, y-1), callback = lambda: myla.move_to((x-1, y-1), callback = lambda: myla.follow(player_one)))
player_one.focus()

croc_0.face_east()
croc_1.face_east()
croc_2.face_east()
croc_3.face_east()

croc_0.check_kill([player_one, myla])
croc_1.check_kill([player_one, myla])
croc_2.check_kill([player_one, myla])
croc_3.check_kill([player_one, myla])

if not came_from_main_b:
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

def leave_beg(player_object):
    player_data.save_and_exit("/world_1")

exit_level_start.player_walked_on = leave_beg


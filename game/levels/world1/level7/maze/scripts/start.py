#commence save-data set-up
world_name = "world1"
level_name = "level7"
map_name = "main"

engine.update_world_text("1")
engine.update_level_text("7")

player_data.load(engine.get_player_name())
player_data.set_map(world_name, level_name = level_name, map_name = map_name) #change the map and save that the map has changed
#end save-data set-up

engine.enable_py_scripter()
player_one.focus()
engine.play_music("world_1_jungle")

checks = []
reached = [False] * (len(checks))

for i in range(len(checks)):
    checks[i].player_walked_on = lambda player_object: player_data.save_checkpoint("check" + str(i))

start_from_beg = True

for i in range(len(checks)):
    if player_data.previous_exit_is(world_name = world_name, level_name = level_name, map_name = map_name, info = "check" + str(i)):
        reached[i] = True
        start_from_beg = False
        break

if start_from_beg:
    player_one.face_west()
    player_one.set_cuts_left(3)
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("This region has a bunch of confused security guards", callback = callback),
        lambda callback: engine.clear_scripter(callback = callback),
        lambda callback: engine.insert_to_scripter("#teach arithmetic", callback = callback),
        lambda callback: player_one.set_busy(False, callback =callback)
    ])

def go_next(player_object):
    player_data.save_and_exit("/world1")

end_level.player_walked_on = go_next

def leave_beg(player_object):
    player_data.save_and_exit("/world1")

exit_level_start.player_walked_on = leave_beg

####################Start actual content
security_three.move_east()
security_three.move_east()
security_three.move_east()
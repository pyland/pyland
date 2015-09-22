#commence save-data set-up
world_name = "world2"
level_name = "demo"
map_name = "main"

player_data.load(engine.get_player_name())
player_data.set_map(world_name, level_name = level_name, map_name = map_name)

start.player_walked_on = lambda player_object: player_data.save_and_exit("/world1")


player_one.set_character_name("QtPi")
player_one.focus()
player_one.face_east()
engine.run_callback_list_sequence([
            lambda callback: engine.clear_scripter(callback = callback),
            lambda callback: engine.show_dialogue("Get to the end of the maze!", callback = callback),
            lambda callback: engine.insert_to_scripter("#Instructions:\n#Use the arrow keys to move around or the following python commands\n#can_move() returns a boolean indicating if you \n\t#can move to the tile in front of you \n#move() moves you to the tile in front of you \n#turn_left() turns you left \n#turn_right() turns you right")
        ])


next_level.player_walked_on = lambda player_object: engine.change_map("demo_levels/level_2/main")

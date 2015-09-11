world_name = "world2"
level_name = "demo2"
map_name = "main"

player_data.load(engine.get_player_name())
player_data.set_map(world_name, level_name = level_name, map_name = map_name)

end.player_walked_on = lambda player_object: player_data.save_and_exit("/world1")


player_one.set_character_name("QtPi")
player_one.focus()
player_one.face_east()

engine.run_callback_list_sequence([
            lambda callback: engine.show_dialogue("Get to the end of the maze...again!", callback = callback),
            lambda callback: engine.show_dialogue("This maze looks really big, scripting may come in handy.", callback = callback),
        ])

end.player_walked_on = lambda player_object: engine.run_callback_list_sequence([
    lambda callback: player_one.set_busy(True, callback = callback),
    lambda callback: engine.show_dialogue("You did it!", callback = callback),
    lambda callback: player_one.set_busy(False)

    ])

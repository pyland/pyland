#Implement your level logic here!

player_one.focus()

player_one.set_character_name("PyPlayer")

engine.play_music("world_1_jungle")

#croc_one.check_kill([player_one,bob])
#croc_two.check_kill([player_one,bob])

croc_one.killable = [player_one, bob]
croc_one.still_check_kill()
croc_two.killable = [player_one, bob]
croc_two.still_check_kill()
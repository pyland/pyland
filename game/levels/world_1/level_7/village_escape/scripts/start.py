player.focus()
engine.update_player_name(engine.get_player_name(), player.get_focus_button_id())

engine.play_music("eery")

engine.update_world_text("1")
engine.update_level_text("7")

engine.clear_scripter()

player.face_east()
myla.face_east()
myla.follow(player)

snakes = [snake0,snake1,snake2,snake3,snake4,snake5,snake6]

snakes[0].change_state("yellow")
snakes[1].change_state("green")
snakes[2].change_state("red")
snakes[3].change_state("green")
snakes[4].change_state("orange")
snakes[5].change_state("orange")
snakes[6].change_state("red")

crocs = [croc_0,croc_1,croc_2,croc_3,croc_4,croc_5,croc_6,croc_7,croc_8]

#for croc in crocs:


for snake in snakes:
    snake.follow(player)

vine_list = [vine_0,vine_1,vine_2,vine_3,vine_4,vine_5,vine_6]

whetstone0.prepare(len(vine_list), vine_list)

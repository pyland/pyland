player.focus()
engine.update_player_name(engine.get_player_name(), player.get_focus_button_id())

engine.play_music("eery")

engine.clear_scripter()

player.face_east()
myla.face_east()
myla.follow(player)

snakes = [snake1,snake2,snake3,snake4,snake5,snake6]

snakes[0].change_state("orange")
snakes[1].change_state("green")
snakes[2].change_state("red")
snakes[3].change_state("green")
snakes[4].change_state("red")
snakes[5].change_state("yellow")

for snake in snakes:
    snake.follow(player)

import random
player_one.focus()
engine.set_py_tabs(2)

treasure_x = random.randint(1, 48)
treasure_y = random.randint(1, 48)

player_one.set_character_name("Player One")
player_two.set_character_name("Player Two")
player_one.face_north()
player_two.face_south()

def dig(player):
    x,y = player.get_position()
    if player.get_position() == (treasure_x, treasure_y):
        engine.show_dialogue("You win!")
        player_one.set_busy(True)
        player_two.set_busy(True)
    else:
        return 5000*((treasure_x - x)*(treasure_x - x) + (treasure_y - y)*(treasure_y - y))


player_one.dig = lambda : dig(player_one)
player_two.dig = lambda : dig(player_two)




def run_all():
    player_one.run_script(script_to_run = 1)
    player_two.run_script(script_to_run = 2)
    player_one.wait(0.2,callback = lambda: player_one.run_script(script_to_run = 1))

engine.run_callback_list_sequence([

    lambda callback: player_one.set_busy(True, callback = callback),
    lambda callback: player_two.set_busy(True, callback = callback),
    lambda callback: engine.show_dialogue("Enter the code! Press enter to start!", False, callback = callback),
    lambda callback: player_one.set_busy(False, callback = callback),
    lambda callback: player_two.set_busy(False, callback = callback),
    lambda callback: run_all()
    ])





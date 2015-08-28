player_one.focus()
engine.set_py_tabs(4)


def focus_func(func):
        """ Wraps functions so that they are only run if the player is the focus """
        return func() if camera.is_focus() else None

engine.register_input_callback(engine.INPUT_UP, lambda: focus_func(camera.move_north))
engine.register_input_callback(engine.INPUT_RIGHT, lambda: focus_func(camera.move_east))
engine.register_input_callback(engine.INPUT_DOWN, lambda: focus_func(camera.move_south))
engine.register_input_callback(engine.INPUT_LEFT, lambda: focus_func(camera.move_west))

camera.focus()

player_one.set_character_name("One")
player_two.set_character_name("Two")
player_three.set_character_name("Three")
player_four.set_character_name("Four")


player_one.face_north()
player_two.face_west()
player_three.face_south()
player_four.face_east()


def run_all():
    monkey.find_escape_path()
    player_one.run_script(script_to_run = 1)
    player_two.run_script(script_to_run = 2)
    player_three.run_script(script_to_run = 3)
    player_four.run_script(script_to_run = 4)
intro_dialogue = [

    lambda callback: player_one.set_busy(True, callback = callback),
    lambda callback: player_two.set_busy(True, callback = callback),
    lambda callback: player_three.set_busy(True, callback = callback),
    lambda callback: player_four.set_busy(True, callback = callback),

    lambda callback: engine.show_dialogue("You have 2 minutes to type any code you might need", callback = callback),
    lambda callback: monkey.wait(5, callback = callback),

    lambda callback: player_one.set_busy(False, callback = callback),
    lambda callback: player_two.set_busy(False, callback = callback),
    lambda callback: player_three.set_busy(False, callback = callback),
    lambda callback: player_four.set_busy(False, callback = callback),

    lambda callback: run_all()
    ]


engine.run_callback_list_sequence(intro_dialogue)





import sys
import os

sys.path.insert(1, "../game/components")
from scoped_interpreter import ScopedInterpreter
 
camera.focus()
c1 = (50, 50, 50)
c2 = (110, 110, 110)
engine.set_ui_colours(c1, c2)
engine.play_music("calm")

dialogue_sequence = [
    lambda callback: engine.show_dialogue(engine.get_dialogue("intro_coming_now"), callback),
    lambda callback: engine.show_dialogue(engine.get_dialogue("intro_im_monty_the_snake"), callback),
    lambda callback: engine.show_dialogue(engine.get_dialogue("intro_monty_doesnt_know_name"), callback),
    lambda callback: engine.show_dialogue(engine.get_dialogue("intro_monty_text_editor"), callback),
    lambda callback: engine.show_dialogue(engine.get_dialogue("intro_big_white_box"), callback),
    lambda callback: engine.show_dialogue(engine.get_dialogue("intro_monty_hello_player", {"player_name": "Alex"}), callback),
    lambda callback: engine.show_dialogue(engine.get_dialogue("intro_wrote_first_program", {"player_name": "Alex"}), callback),
    lambda callback: engine.show_dialogue(engine.get_dialogue("intro_console_output"), callback),
]

engine.run_callback_list_sequence(dialogue_sequence)

#engine.print_terminal(player_one.get_position(), False)
#camera.move_to_position(position, time)
camera.move_west(lambda: camera.move_west(lambda: camera.move_west(lambda: camera.move_east(lambda: camera.move_east()))))

def get_name(name):
    engine.print_terminal(name)

imbued_locals_name = {"print" : get_name}

#camera.wait(2, lambda: engine.print_terminal("h"))


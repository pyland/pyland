import sys
import os

sys.path.insert(1, "../game/components")
from scoped_interpreter import ScopedInterpreter

config = engine.get_config()

camera.focus()
c1 = (50, 50, 50)
c2 = (110, 110, 110)
engine.set_ui_colours(c1, c2)
engine.play_music("calm")

def get_player_name(callback):
    player_api = {
        "print" : lambda text: engine.print_terminal(text)
    }
    scoped_interpreter = ScopedInterpreter(player_api, lambda error_output: engine.print_terminal(error_output, True))
    script_filename = os.path.dirname(config['files']['player_scripts'] + "/1.py")
    #open and read the script
    with open(script_filename, encoding="utf8") as script_file:
                script = script_file.read()


    #engine.register_input_callback(engine.INPUT_RUN, focus_func(""))



dialogue_sequence = [
    lambda callback: engine.show_dialogue(engine.get_dialogue("intro_coming_now"), callback),
    lambda callback: camera.move_by((0, -11), 2.2, callback),
    lambda callback: camera.wait(0.2, callback),
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

def get_name(name):
    engine.print_terminal(name)

imbued_locals_name = {"print" : get_name}

#camera.wait(2, lambda: engine.print_terminal("h"))


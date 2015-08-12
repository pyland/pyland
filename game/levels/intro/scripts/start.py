import sys
import os

sys.path.insert(1, engine.get_config()['files']['components_location'])
from script_state_container import ScriptStateContainer

config = engine.get_config()

camera.focus()
c1 = (210, 210, 210)
c2 = (190, 190, 190)
engine.set_ui_colours(c1, c2)
engine.play_music("calm")

def get_player_name(callback):
    script_api = {
        "print" : lambda text: engine.print_terminal(text)
    }

    stc = ScriptStateContainer()
    
    stc.set_script_name("GetName")
    engine.register_input_callback(engine.INPUT_RUN, lambda: stc.run_script(script_api, engine))
    engine.register_input_callback(engine.INPUT_HALT, stc.halt_script)



dialogue_sequence = [
    lambda callback: engine.show_dialogue(engine.get_dialogue("intro_coming_now"), callback),
    lambda callback: camera.move_by((0, -11), 2.2, callback),
    lambda callback: camera.wait(0.2, callback),
    lambda callback: engine.show_dialogue(engine.get_dialogue("intro_im_monty_the_snake"), callback),
    lambda callback: engine.show_dialogue(engine.get_dialogue("intro_monty_doesnt_know_name"), callback),
    lambda callback: get_player_name(callback),
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


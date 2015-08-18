import sys

sys.path.insert(1, engine.get_config()['files']['script_running_location'])
from script_state_container import ScriptStateContainer

config = engine.get_config()

camera.focus()
c1 = (210, 210, 210)
c2 = (190, 190, 190)
engine.set_ui_colours(c1, c2)
engine.play_music("calm")


""" Some quick and hacky methods I wrote to grab the player's name from the terminal"""
player_name = "???"

name_parsed = False

engine.clear_scripter()
engine.set_py_tabs(1)
engine.disable_py_scripter()

def name_parser(name, callback):
    global name_parsed
    global player_name
    name = str(name)
    engine.print_terminal(name)
    if not name_parsed:
        name_parsed = True
        player_name = name
        engine.clear_scripter()
        engine.disable_py_scripter()
        callback()

def get_player_name(callback):
    engine.clear_scripter()
    engine.insert_to_scripter("print(\"YourName\")")
    engine.enable_py_scripter()
    script_api = {
        "print" : lambda text: name_parser(text, callback)
    }

    stc = ScriptStateContainer()

    stc.set_script_name("GetName")
    engine.register_input_callback(engine.INPUT_RUN, lambda: stc.run_script(script_api, engine))
    engine.register_input_callback(engine.INPUT_HALT, stc.halt_script)


level_name = "world_1/intro"
dialogue_sequence = [
    lambda callback: engine.show_dialogue(engine.get_dialogue(level_name, "monty_coming_now"), callback = callback),
    lambda callback: camera.move_by((0, -11), 2.2, callback = callback),
    lambda callback: camera.wait(0.2, callback = callback),
    lambda callback: engine.show_dialogue(engine.get_dialogue(level_name, "im_monty_the_snake"), callback = callback),
    lambda callback: engine.show_dialogue(engine.get_dialogue(level_name, "monty_doesnt_know_name"), callback = callback),
    lambda callback: engine.show_dialogue(engine.get_dialogue(level_name, "monty_text_editor"), callback = callback),
    lambda callback: engine.show_dialogue(engine.get_dialogue(level_name, "big_white_box"), callback = callback),
    lambda callback: get_player_name(callback = callback),
    lambda callback: engine.show_dialogue(engine.get_dialogue(level_name, "confirm_player_name", {"player_name": player_name}), callback = callback), #TODO, give option for dictionary of callback response
    lambda callback: engine.show_dialogue(engine.get_dialogue(level_name, "wrote_first_program", {"player_name": player_name}), callback = callback),
    lambda callback: engine.show_dialogue(engine.get_dialogue(level_name, "console_output"), callback = callback),
    lambda callback: engine.show_dialogue(engine.get_dialogue(level_name, "go_enjoy_pyland", {"player_name": player_name}), callback = callback)
    #TODO: save the player's name in a new save file for them :)
    #TODO: change the level to the intro level
]

engine.run_callback_list_sequence(dialogue_sequence)

"""
engine.run_callback_list_sequence(
    dialogue_sequence,
    lambda: engine.show_dialogue_with_options(
        engine.get_dialogue(level_name, "confirm_player_name", {"player_name": player_name}),
        {
            "Yes": "",
            "No" : ""
        }
    )
)
"""


import sys
import json

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
        if(engine.save_exists(name)):
            engine.print_terminal("A save with the name: " + name + " already exists!") #TODO: handle this a lot more cleverly!
        engine.set_player_name(name)
        engine.disable_py_scripter()
        engine.flush_input_callback_list(engine.INPUT_RUN)
        engine.flush_input_callback_list(engine.INPUT_HALT)
        callback()

def get_player_name(callback):
    global name_parsed
    name_parsed = False
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


level_name = "/world_1/intro"
introduction_sequence = [
    lambda callback: engine.show_dialogue(engine.get_dialogue(level_name, "monty_coming_now"), callback = callback),
    lambda callback: camera.move_by((0, -11), 2.2, callback = callback),
    lambda callback: camera.wait(0.2, callback = callback),
    lambda callback: engine.show_dialogue(engine.get_dialogue(level_name, "im_monty_the_snake"), callback = callback),
    lambda callback: engine.show_dialogue(engine.get_dialogue(level_name, "monty_doesnt_know_name"), callback = callback),
    lambda callback: engine.show_dialogue(engine.get_dialogue(level_name, "monty_text_editor"), callback = callback),
    lambda callback: engine.show_dialogue(engine.get_dialogue(level_name, "big_white_box"), callback = callback),
    lambda callback: get_player_name(callback = callback),
    #TODO: save the player's name in a new save file for them :)
    #TODO: change the level to the intro level
]

def confirm_name():
    engine.show_dialogue_with_options(
        engine.get_dialogue(level_name, "confirm_player_name", {"player_name": player_name}),
        {
            "Yes": lambda: engine.run_callback_list_sequence(name_confirmed_sequence, start_game), #change the level once the intro has finished
            "No" : lambda: engine.run_callback_list_sequence(name_wrong_sequence, confirm_name)
        }
    )

engine.run_callback_list_sequence(introduction_sequence, callback = confirm_name)

name_wrong_sequence = [
    lambda callback: engine.show_dialogue(engine.get_dialogue(level_name, "player_name_wrong"), callback = callback),
    lambda callback: get_player_name(callback = callback)
]

name_confirmed_sequence = [
    lambda callback: engine.show_dialogue(engine.get_dialogue(level_name, "wrote_first_program", {"player_name": player_name}), callback = callback),
    lambda callback: engine.show_dialogue(engine.get_dialogue(level_name, "console_output"), callback = callback),
    lambda callback: engine.show_dialogue(engine.get_dialogue(level_name, "go_enjoy_pyland", {"player_name": player_name}), callback = callback)
]

def start_game():
    """Save the player's game and start the game!"""
    save_string = ""
    with open(engine.get_config()['files']['initial_save_location'], encoding="utf8") as initial_save_file:
        save_string = initial_save_file.read()
    json_data = json.loads(save_string)
    engine.save_player_data(engine.get_player_name(), json_data)
    engine.change_map("/world_1/level_1/player_house")


import sys

sys.path.insert(1, engine.get_config()['files']['components_location'])
from script_state_container import ScriptStateContainer

config = engine.get_config()

camera.focus()
c1 = (210, 210, 210)
c2 = (190, 190, 190)
engine.set_ui_colours(c1, c2)
engine.play_music("calm")

engine.clear_scripter()

""" Some quick and hacky methods I wrote to grab the player's name from the terminal"""
player_name = "???"

name_parsed = False

def name_parser(name, callback):
    global name_parsed
    global player_name
    name = str(name)
    engine.print_terminal(name)
    if(name_parsed):
        pass #TODO: handle this
    else:
        name_parsed = True
        player_name = name
        callback()

def get_player_name(callback):
    engine.clear_scripter()
    engine.insert_to_scripter("print(\"YourName\")")
    script_api = {
        "print" : lambda text: name_parser(text, callback)
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
    lambda callback: engine.show_dialogue(engine.get_dialogue("intro_monty_text_editor"), callback),
    lambda callback: engine.show_dialogue(engine.get_dialogue("intro_big_white_box"), callback),
    lambda callback: get_player_name(callback),
    lambda callback: engine.show_dialogue(engine.get_dialogue("intro_confirm_player_name", {"player_name": player_name}), callback), #TODO, give option for dictionary of callback response
    lambda callback: engine.show_dialogue(engine.get_dialogue("intro_wrote_first_program", {"player_name": player_name}), callback),
    lambda callback: engine.show_dialogue(engine.get_dialogue("intro_console_output"), callback),
    lambda callback: engine.show_dialogue(engine.get_dialogue("intro_go_enjoy_pyland", {"player_name": player_name}), callback)
    #TODO: save the player's name in a new save file for them :)
    #TODO: change the level to the intro level
]

engine.run_callback_list_sequence(dialogue_sequence)


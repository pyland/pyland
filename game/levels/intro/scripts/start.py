import sys
import os

sys.path.insert(1, "../game/components")
from scoped_interpreter import ScopedInterpreter
 
camera.focus();
engine.play_music("calm")
engine.add_dialogue(engine.getDialogue("intro_coming_now"))
engine.add_dialogue(engine.getDialogue("intro_im_monty_the_snake"))
engine.add_dialogue(engine.getDialogue("intro_monty_doesnt_know_name"))
engine.add_dialogue(engine.getDialogue("intro_monty_text_editor"))
engine.add_dialogue(engine.getDialogue("intro_big_white_box"))
engine.open_dialogue_box() #Give callback here?

#engine.print_terminal(player_one.get_position(), False)
#camera.move_to_position(position, time)
camera.move_west(lambda: camera.move_west(lambda: camera.move_west(lambda: camera.move_east(lambda: camera.move_east()))))

def get_name(name):
    engine.print_terminal(name)

imbued_locals_name = {"print" : get_name}

#camera.wait(2, lambda: engine.print_terminal("h"))


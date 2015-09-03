#Myla will teach you that PyScripter can be used to move
#She gives you scripts, so you know how to fix the other's scripts

#commence save-data set-up
world_name = "world_1"
level_name = "level_3"
map_name = "main"

engine.update_world_text("1")
engine.update_level_text("3")

engine.set_py_tabs(2)

player_one.focus()
player_one.face_south()

horizontal_crocodiles = [croc_0,croc_1,croc_3,croc_4]

vertical_crocodiles = [croc_5,croc_6,croc_7,croc_8,croc_9,croc_10]

croc_2.face_west()

for croc in horizontal_crocodiles:
    croc.face_east()
    croc.change_state("swim")
    croc.move_horizontal(player_one)

for croc in vertical_crocodiles:
    croc.face_south()
    croc.change_state("swim")
    croc.move_vertical(player_one)

def write_script(challenger):
    challenger.turn_to_face(player_one)
    engine.run_callback_list_sequence([
        lambda callback: challenger.set_busy(False, callback = callback),
        lambda callback: engine.show_external_script(
            confirm_callback = lambda: engine.run_callback_list_sequence([
                lambda callback: player_one.set_busy(False, callback = callback),
                lambda callback: challenger.run_script(script_to_run = 10)]),
            cancel_callback = lambda: player_one.set_busy(False),
            external_dialogue = "I will be have script for you to fix but for now I'll run your script!",
            script_init = lambda: engine.insert_to_scripter("")
        )
    ])


npc_0.player_action = lambda player_object: write_script(npc_0)
npc_1.player_action = lambda player_object: write_script(npc_1)
npc_2.player_action = lambda player_object: write_script(npc_2)
npc_3.player_action = lambda player_object: write_script(npc_3)
npc_4.player_action = lambda player_object: write_script(npc_4)

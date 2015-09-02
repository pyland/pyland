#commence save-data set-up
world_name = "world_1"
level_name = "level_3"
map_name = "main"

engine.update_world_text("1")
engine.update_level_text("3")

engine.set_py_tabs(2)

player_one.focus()
player_one.face_south()

crocodiles = [croc_0,croc_1]
for croc in crocodiles:
    croc.face_east()
    croc.change_state("swim")
    croc.move_horizontal(player_one)

#challenge_one_croc = [c1cr0]
#for croc in challenge_one_croc:
#    croc.face_east()
#    croc.change_state("swim")


def write_script(challenger):
    challenger.turn_to_face(player_one)
    engine.run_callback_list_sequence([
        lambda callback: challenger.set_busy(False, callback = callback),
        lambda callback: engine.show_external_script(
            confirm_callback = lambda: engine.run_callback_list_sequence([
                lambda callback: player_one.set_busy(False, callback = callback),
                lambda callback: challenger.run_script(script_to_run = 10)]),
            cancel_callback = lambda: player_one.set_busy(False),
            external_dialogue = "Can you fix my script?",
            script_init = lambda: engine.insert_to_scripter("mve_east()")
        )
    ])


npc_0.player_action = lambda player_object: write_script(npc_0)
npc_1.player_action = lambda player_object: write_script(npc_1)
npc_2.player_action = lambda player_object: write_script(npc_2)
challenge_3.player_action = lambda player_object: challenger_action(player_object, challenge_3)
challenge_4.player_action = lambda player_object: challenger_action(player_object, challenge_4)
challenge_5.player_action = lambda player_object: challenger_action(player_object, challenge_5)

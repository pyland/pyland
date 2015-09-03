#commence save-data set-up
world_name = "world_2"
level_name = "level_1"
map_name = "main"

engine.update_world_text("2")
engine.update_level_text("1")

player_one.focus()
player_one.face_south()

challenge_three_croc = [c3cr0, c3cr1, c3cr2, c3cr3, c3cr4]
for croc in challenge_three_croc:
    croc.face_north()
    croc.change_state("swim")

challenge_one_croc = [c1cr0]
for croc in challenge_one_croc:
    croc.face_east()
    croc.change_state("swim")


challenge_0.face_west()
challenge_0.set_cuts_left(1)
challenge_1.face_west()
challenge_2.face_south()
challenge_3.face_south()
challenge_3.set_cuts_left(5)
challenge_4.face_north()
challenge_4.set_cuts_left(1)
challenge_5_0.face_west()
challenge_5_0.set_cuts_left(1)
challenge_5_1.face_east()

def challenger_action(player_object, challenger):
    challenger.turn_to_face(player_object)
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: challenger.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("My PyRunner doesn't seem to do anything. Can you have a look at it please?.", callback = callback),
        lambda callback: write_script(challenger)
    ])
    """,
    callback = lambda: engine.show_dialogue_with_options(
        "Can you help me?",
        options = {
            "Yes": lambda: write_script(challenger),
            "No" : lambda: engine.run_callback_list_sequence([
                lambda callback: player_one.set_busy(False, callback = callback),
                lambda callback: engine.show_dialogue("Aw, that's a shame", callback = callback),])
        }))"""



def write_script(challenger):
    engine.run_callback_list_sequence([
        lambda callback: challenger.set_busy(False, callback = callback),
        lambda callback: engine.show_external_script(
            confirm_callback = lambda: engine.run_callback_list_sequence([
                lambda callback: player_one.set_busy(False, callback = callback),
                lambda callback: challenger.run_script(script_to_run = 10)]),
            cancel_callback = lambda: player_one.set_busy(False),
            external_dialogue = "Why doesn't this do anything?",
            script_init = lambda: engine.insert_to_scripter(""),
            character_object = challenger
        )
    ])


challenge_0.player_action = lambda player_object: challenger_action(player_object, challenge_0)
challenge_1.player_action = lambda player_object: challenger_action(player_object, challenge_1)
challenge_2.player_action = lambda player_object: challenger_action(player_object, challenge_2)
challenge_3.player_action = lambda player_object: challenger_action(player_object, challenge_3)
challenge_4.player_action = lambda player_object: challenger_action(player_object, challenge_4)
challenge_5_0.player_action = lambda player_object: challenger_action(player_object, challenge_5_0)
challenge_5_1.player_action = lambda player_object: challenger_action(player_object, challenge_5_1)


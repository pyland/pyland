#commence save-data set-up
world_name = "world_2"
level_name = "level_3"
map_name = "main"
player_one.focus()

challenge_one_croc = [croc_0, croc_1, croc_2, croc_3, croc_4]
for croc in challenge_one_croc:
    croc.face_north()
    croc.change_state("swim")

challenge_one.face_south()

def challenge_one_action(player_object):
    challenge_one.turn_to_face(player_object)
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("My PyRunner doesn't seem to do anything. Can you have a look at it please?.", callback = callback),
    ],
    callback = lambda: engine.show_dialogue_with_options(
        "Can you help me?",
        options = {
            "Yes": lambda: write_script(challenge_one),
            "No" : lambda: engine.run_callback_list_sequence([
                lambda callback: player_one.set_busy(False, callback = callback),
                lambda callback: engine.show_dialogue("Aw, that's a shame", callback = callback),])
        }))
            


def write_script(challenger):
    engine.run_callback_list_sequence([
        lambda callback: engine.show_dialogue("Thank you! Help me run a script!",  callback = callback),
        lambda callback: engine.show_external_script(
            confirm_callback = lambda: engine.run_callback_list_sequence([
                lambda callback: player_one.set_busy(False, callback = callback),
                lambda callback: challenger.run_script(script_to_run = 10)]),
            cancel_callback = lambda: player_one.set_busy(False),
            external_dialogue = "Why doesn't this do anything?",
            script_init = lambda: engine.insert_to_scripter("")
        )
    ])



challenge_one.player_action = challenge_one_action
challenge_one.set_cuts_left(5)
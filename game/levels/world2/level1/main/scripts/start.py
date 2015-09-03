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
    croc.killable = [player_one, lily]
    croc.still_check_kill()

challenge_one_croc = [c1cr0]
for croc in challenge_one_croc:
    croc.face_east()
    croc.change_state("swim")
    croc.killable = [player_one, lucas, sam]
    croc.still_check_kill()

challengers = [matthew,lucas,sam, lily, susan,julie,courtney]


challengers[0].face_west()
challengers[0].set_cuts_left(1)
challengers[1].face_west()
challengers[2].face_south()
challengers[3].face_south()
challengers[3].set_cuts_left(5)
challengers[4].face_north()
challengers[4].set_cuts_left(1)
challengers[5].face_west()
challengers[5].set_cuts_left(1)
challengers[6].face_east()

def challenger_action(player_object, challenger):
    challenger.turn_to_face(player_object)
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: challenger.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("Take a look at this cool device I found!.", callback = callback),
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
            external_dialogue = "Isn't this neat?",
            #script_init = lambda: engine.insert_to_scripter(""),
            character_object = challenger
        )
    ])


challengers[0].player_action = lambda player_object: challenger_action(player_object, challengers[0])
challengers[1].player_action = lambda player_object: challenger_action(player_object, challengers[1])
challengers[2].player_action = lambda player_object: challenger_action(player_object, challengers[2])
challengers[3].player_action = lambda player_object: challenger_action(player_object, challengers[3])
challengers[4].player_action = lambda player_object: challenger_action(player_object, challengers[4])
challengers[5].player_action = lambda player_object: challenger_action(player_object, challengers[5])
challengers[6].player_action = lambda player_object: challenger_action(player_object, challengers[6])


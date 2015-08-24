#global settings for level
engine.play_music("eery")
engine.set_ui_colours((200,255,200),(215,255,215)) #TODO: save these colours in the config.
engine.set_py_tabs(9)

#initial setting of players
player_one.face_south()
player_one.focus()
myla.follow(player_one)

#beginning of level
engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("Let's see what mysteries this forest contains...", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback)
    ])

#what happens when you stand on a vine
def vine_die_action():
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("Aagh! The vines got you!", callback = callback)
    ])

#player meets vine 1
met_vine1 = False

def meet_vine1():
    global met_vine1
    if not met_vine1:
        met_vine1 = True
        vine1_sequence = [
            lambda callback: player_one.set_busy(True, callback = callback),
            lambda callback: engine.show_dialogue("Watch out! Don't step on that vine! It's spiky and poisonous." , callback = callback),
            lambda callback: player_one.set_busy(False, callback = callback)
        ]
        engine.run_callback_list_sequence(vine1_sequence)

vine1.grow(player_one)
vine1.contact_action = vine_die_action

trigger1a.player_walked_on = lambda player_object: meet_vine1()
trigger1b.player_walked_on = lambda player_object: meet_vine1()

#player meets vine 2
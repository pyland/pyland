player_one.focus()

engine.play_music("eery")
engine.set_ui_colours((200,255,200),(215,255,215)) #TODO: save these colours in the config.
engine.set_py_tabs(9)


height = 7
width = 20

def vine_die_action():
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("Aagh! The vines got you!", callback = callback)
    ])

def vine_start():
    for i in range(height):
        for j in range(width):
            vinename = "vines_" + str(i) + "_" + str(j)
            command_grow = vinename + ".wait(" + str(1.40*i) + ", lambda : " + vinename + ".grow(player_one))"
            command_die = vinename + ".die_action = vine_die_action"
            exec(command_die)
            exec(command_grow)

engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("There are poisonous vines growing in this forest, we better run away from them!", callback = callback),
        lambda callback: engine.show_dialogue("Let me get into your bag cause I can't run like you...", callback = callback),
        lambda callback: player_one.face_east(callback = callback),
        lambda callback: myla.move_east(callback = callback),
        lambda callback: myla.set_visible(False, callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback),
    ], callback = lambda: vine_start())

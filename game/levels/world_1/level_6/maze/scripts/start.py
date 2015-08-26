player_one.focus()
myla.follow(player_one)

engine.play_music("eery")
engine.set_ui_colours((200,255,200),(215,255,215)) #TODO: save these colours in the config.
engine.set_py_tabs(9)

whetstone1.prepare(12, [])
height = 60
width = 40

def vine_init():
    for i in range(height):
        for j in range(width):
            vinename = "vine_" + str(i) + "_" + str(j)
            command_vis = vinename + ".set_visible(False)"
            exec(command_vis)

vine_init()

def vine_die_action():
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("Aagh! The vines got you!", callback = callback)
    ])

def vine_start():
    for i in range(height):
        for j in range(width):
            vinename = "vine_" + str(i) + "_" + str(j)
            command_grow = vinename + ".wait(" + str(2.00*i) + ", lambda : " + vinename + ".grow())"
            command_die = vinename + ".contact_action = vine_die_action"
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

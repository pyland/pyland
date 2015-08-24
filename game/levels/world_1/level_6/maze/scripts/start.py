player_one.focus()
myla.follow(player_one)

engine.play_music("eery")
engine.set_ui_colours((200,255,200),(215,255,215)) #TODO: save these colours in the config.
engine.set_py_tabs(9)

#for i in range(40):
"""
vinename = "vines0" + str(i)
grow_vine = vinename + ".wait(12.5, lambda : vinename.set_visible(True))"
exec(grow_vine)
"""

height = 7
width = 20

def vine_die_action():
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("Aagh! The vines got you!", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback),
    ])


for i in range(height):
    for j in range(width):
        vinename = "vines_" + str(i) + "_" + str(j)
        command_grow = vinename + ".wait(" + str(1.40*i) + ", lambda : " + vinename + ".grow(player_one))"
        command_die = vinename + ".die_action = vine_die_action"
        exec(command_die)
        exec(command_grow)


#vines00.wait(0.40, lambda : vines00.grow(player_one))
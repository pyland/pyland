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

for i in range(height):
    for j in range(width):
        vinename = "vines_" + str(i) + "_" + str(j)
        #engine.print_terminal(vinename)
        command = vinename + ".wait(" + str(0.70*i) + ", lambda : " + vinename + ".grow(player_one))"
        engine.print_terminal(command) 
        exec(command)

"""
vinename = "vines" + str(0) + str(0)
engine.print_terminal(vinename)
command = vinename + ".wait(0.40, lambda : " + vinename + ".grow(player_one))"
engine.print_terminal(command) 
exec(command)
"""
#vines00.wait(0.40, lambda : vines00.grow(player_one))
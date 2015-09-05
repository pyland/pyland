#Heidi is with you going to find this secret device
#She's scared off by the crocodiles but you still go
#Pick it up and meet Myla


#commence save-data set-up
world_name = "world1"
level_name = "level1"
map_name = "main"

engine.disable_py_scripter()
myla.set_visible(False)

player_data.load(engine.get_player_name())
player_data.set_map(world_name, level_name = level_name, map_name = map_name) #change the map and save that the map has changed

end.player_walked_on = lambda player_object: player_data.save_and_exit("/world1")

engine.update_world_text("1")
engine.update_level_text("1")
engine.set_ui_colours((200,255,200),(215,255,215))

player_one.focus()
engine.play_music("beach")

b1_croc = [
#croc_0,
croc_1,
croc_2,
croc_3,
#croc_4,
croc_5,
croc_6,
croc_7,
croc_8,
croc_9]

for i in range(len(b1_croc)):
    croc = b1_croc[i]
    croc.killable = [player_one]
    croc.change_state("swim")

    if i % 2 == 0:
        croc.face_east()
    else:
        croc.face_west()
    croc.move_horizontal()

engine.run_callback_list_sequence([
    lambda callback: player_one.set_busy(True, callback = callback),
    lambda callback: camera.focus(callback = callback),
    lambda callback: engine.show_dialogue("Attention Pyland Citizens: Use the arrow keys to move your player", ignore_scripting = True, callback = callback),
    lambda callback: player_one.focus(callback = callback),
    lambda callback: player_one.set_busy(False, callback = callback),
])

start_callback = [
        lambda callback: player_one.focus(callback = callback),
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: heidi.turn_to_face(player_one, callback = callback),
        lambda callback: engine.show_dialogue("Heidi: So you think you there's a magical scripting device across the river huh?", ignore_scripting = True, callback = callback),
        lambda callback: engine.show_dialogue("Looks pretty dangerous but if you insist...", ignore_scripting = True, callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback),
        lambda callback: heidi.start_turning(1.0, 7, callback = callback),
]


done_initial = False

def initial_info():
    global done_initial
    if (not done_initial) :
        engine.run_callback_list_sequence(start_callback)
        done_initial = True

heidi_talk.player_walked_on = lambda player_object: initial_info()

heidi_callback = [
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: heidi.stop_turning(callback = callback),
        lambda callback: heidi.turn_to_face(player_one, callback = callback),
        lambda callback: engine.show_dialogue("You think you can make it across the bridge?!", ignore_scripting = True, callback = callback),
        lambda callback: heidi.start_turning(1.0, 7, callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback),
]



heidi.player_action  = lambda player_object: engine.run_callback_list_sequence(heidi_callback)

croc_callback = [
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: heidi.turn_to_face(player_one, callback = callback),
        lambda callback: engine.show_dialogue("Heidi: And I forgot to mention, don't stand next to any crocodiles...", ignore_scripting = True, callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback),
]

done_warning = False

def croc_info():
    global done_warning
    if (not done_warning) :
        engine.run_callback_list_sequence(croc_callback)
        done_warning = True

croc_warning.player_walked_on = lambda player_object: croc_info()

made_it_callback = [
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("Hit 'Enter' to pick up items", ignore_scripting = True, callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback),
]

done_made_it = False

def made_it_info():
    global done_made_it
    if (not done_made_it) :
        engine.run_callback_list_sequence(made_it_callback)
        done_made_it = True

made_it.player_walked_on = lambda player_object: made_it_info()

#defining the possible states of the level
pyscripter_state_not_found = 0
pyscripter_state_found = 1
#end state definitions

pyscripter_state = pyscripter_state_not_found
#pyscripter_state = player_data.get_level_state("pyscripter_state")
if pyscripter_state == pyscripter_state_not_found:
    pyscripter.move_to((14, 69))
    def pyscripter_player_action(player_object):
        engine.run_callback_list_sequence([
            lambda callback: player_one.set_busy(True, callback = callback),
            lambda callback: pyscripter.set_visible(False, callback = callback),
            lambda callback: pyscripter.set_solidity(False, callback = callback),
            lambda callback: pyscripter.move_to((0, 0), callback = callback),
            lambda callback: engine.show_dialogue(engine.get_player_name() + " picked up the PyScripter", callback = callback),
            lambda callback: engine.enable_py_scripter(callback = callback),
            lambda callback: engine.clear_scripter(callback = callback),
            lambda callback: myla.set_visible(True, callback = callback),
            lambda callback: myla.face_south(callback = callback),
            lambda callback: myla.set_solidity(False, callback = callback),
            lambda callback: myla.start_animating(callback = callback),
            lambda callback: myla.move_to((14,69), time = 0.8, callback = callback),
            lambda callback: myla.stop_animating(callback = callback),
            lambda callback: myla.turn_to_face(player_one, callback = callback),
            lambda callback: engine.show_dialogue("???: You found my PyScripter!", callback = callback),
            lambda callback: engine.play_music("world_1_myla", callback = callback),
            lambda callback: engine.show_dialogue("By the way I'm Myla, nice to meet you.", callback = callback),
            lambda callback: engine.show_dialogue("Let's get out of here. These crododiles sure like to cause trouble.", callback = callback),
            lambda callback: myla.follow(player_one, callback = callback),
            lambda callback: player_one.set_busy(False, callback = callback)
        ], save_pyscripter_state)

    def save_pyscripter_state():
        player_data.set_level_state("pyscripter_state", pyscripter_state_found)
        player_data.unlock_pyscripter()
        player_data.save()

    pyscripter.player_action = pyscripter_player_action

elif pyscripter_state == pyscripter_state_found:
    pyscripter.set_visible(False)
    pyscripter.set_solidity(False, callback = lambda: pyscripter.move_to((0,0)))
    annoying_croc.set_visible(False)
    annoying_croc.set_solidity(False)
    myla.move_to(myla_start_position)
    myla.follow(player_one)






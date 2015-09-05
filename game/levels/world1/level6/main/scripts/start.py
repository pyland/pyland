#TODO: Restore Myla properly when reloading checkpoints

#commence save-data set-up
world_name = "world1"
level_name = "level6"
map_name = "main"

engine.update_world_text("1")
engine.update_level_text("6")

player_data.load(engine.get_player_name())
player_data.set_map(world_name, level_name = level_name, map_name = map_name) #change the map and save that the map has changed
#end save-data set-up

engine.enable_py_scripter()
player_one.focus()
engine.play_music("world_1_jungle")

checks = [check1, check2]
reached = [False] * (len(checks))

for i in range(len(checks)):
    checks[i].player_walked_on = lambda player_object: player_data.save_checkpoint("check" + str(i))

start_from_beg = True

for i in range(len(checks)):
    if player_data.previous_exit_is(world_name = world_name, level_name = level_name, map_name = map_name, info = "check" + str(i)):
        reached[i] = True
        start_from_beg = False
        break

if reached[0] == True:
    x,y = check1.get_position()
    player_one.set_cuts_left(0)
    player_one.move_to((x,y), callback = lambda: player_one.face_east())
    myla.move_to((x+1,y))
    myla.follow(player_one)

elif reached[1] == True:
    x,y = check2.get_position()
    player_one.set_cuts_left(0)
    player_one.move_to((x,y), callback = lambda: player_one.face_west())
    myla.move_to((x+1,y))
    myla.follow(player_one)

if start_from_beg:
    player_one.face_south()
    player_one.set_cuts_left(3)
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: myla.turn_to_face(player_one, callback = callback),
        lambda callback: engine.show_dialogue("There are a lot of vines blocking the path here. Never fear, I've got just the right script for you!", callback = callback),
        lambda callback: engine.clear_scripter(callback = callback),
        lambda callback: engine.insert_to_scripter("cut()", callback = callback),
        lambda callback: engine.show_dialogue("Run this to cut them down:", callback = callback),
        lambda callback: myla.follow(player_one, callback = callback),
        lambda callback: player_one.set_busy(False, callback =callback)
    ])

def go_next(player_object):
    player_data.save_and_exit("/world1")

end_level.player_walked_on = go_next

def leave_beg(player_object):
    player_data.save_and_exit("/world1")

exit_level_start.player_walked_on = leave_beg

####################Start actual content

vine_list = [
    vine0,
    vine1,
    vine2,
    vine3,
    vine4,
    vine5a,
    vine5b,
    vine5c,
    vine6,
    vine7a,
    vine7b,
    vine7c,
    vine7d,
    vine7e,
    vine7f,
    vine7g,
    vine8
]

croc_still_list = [
    croc1,
    croc2,
    croc3
]

####
explained_cut = False
def explain_cut(player_object):
    global explained_cut

    if explained_cut:
        return

    explained_cut = True
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("Your knife gets blunt after cutting down vines. I've got another script for that.", callback = callback),
        lambda callback: engine.show_dialogue("The text after the # symbols are comments, the PyScripter ignores them but I've put them there to explain what each line means!", callback = callback),
        lambda callback: engine.clear_scripter(callback = callback),
        lambda callback: engine.insert_to_scripter("#for cutting down an object in front of you\ncut()\n#for getting the number of cuts left \nget_cuts_left()\n# use print() to see the value returned by get_cuts_left()\n\n#From Myla :)", callback = callback),
        lambda callback: player_one.set_busy(False, callback =callback)
    ])
explain_cut_trigger.player_walked_on = explain_cut

####
explained_whetstone = False
def explain_whetstone(player_object):
    global explained_whetstone

    if explained_whetstone:
        return

    explained_whetstone = True
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("Oh look a whestone. You can press 'Enter' on it to sharpen your knife and get more cuts.", callback = callback),
        lambda callback: engine.show_dialogue("Since sharpening knives takes so long, the vines will grow back by the time you finish.", callback = callback),
        lambda callback: player_one.set_busy(False, callback =callback)
    ])
explain_whetstone_trigger.player_walked_on = explain_whetstone

whetstone1.prepare(2, vine_list)

whetstone2.prepare(3, vine_list)

def alpha_speak():
    engine.run_callback_list_sequence([
        lambda callback: alpha.set_busy(False, callback = callback),
        lambda callback: alpha.turn_to_face(player_one, callback = callback),
        lambda callback: engine.show_external_script(
            confirm_callback = lambda: engine.run_callback_list_sequence([
                lambda callback: player_one.set_busy(False, callback = callback),
                lambda callback: alpha.run_script(script_to_run = 10)]),
            cancel_callback = lambda: player_one.set_busy(False),
            external_dialogue = "Give me a script and I will run it!",
            script_init = lambda: engine.insert_to_scripter(""),
            character_object = alpha
            )
    ])
alpha.face_south()
alpha.player_action = lambda player_object: alpha_speak()

for croc in croc_still_list:
    croc.killable = [player_one]
    croc.still_check_kill()

whetstone3.prepare(3, vine_list)

explained_poison = False
def explain_poison(player_object):
    global explained_poison

    if explained_poison:
        return

    explained_poison = True
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: myla.turn_to_face(player_one, callback = callback),
        lambda callback: engine.show_dialogue("That vine up ahead is poisonous to humans. Don't step on it!", callback = callback),
        lambda callback: player_one.set_busy(False, callback =callback)
    ])
explain_poison_trigger.player_walked_on = explain_poison

scroll1.set_message("Other characters in the game may have knives as well,\nthough they may not be sharp.")

def bravo_speak():
    engine.run_callback_list_sequence([
        lambda callback: bravo.set_busy(False, callback = callback),
        lambda callback: bravo.turn_to_face(player_one, callback = callback),
        lambda callback: engine.show_external_script(
            confirm_callback = lambda: engine.run_callback_list_sequence([
                lambda callback: player_one.set_busy(False, callback = callback),
                lambda callback: bravo.run_script(script_to_run = 10)]),
            cancel_callback = lambda: player_one.set_busy(False),
            external_dialogue = "You can edit my script and I will run it!",
            script_init = lambda: engine.insert_to_scripter(""),
            character_object = bravo
            )
    ])
bravo.face_north()
bravo.set_cuts_left(1)
bravo.player_action = lambda player_object: bravo_speak()

again = [
    lambda callback: engine.show_dialogue("Here you go!", callback = callback),
    lambda callback: engine.clear_scripter(callback = callback),
    lambda callback: engine.insert_to_scripter("#for cutting down an object in front of you\ncut()\n#for getting the number of cuts left \nget_cuts_left()\n# use print() to see the value returned by get_cuts_left()\nprint(get_cuts_left())\n\n#From Myla :)", callback = callback),
    lambda callback: player_one.set_busy(False, callback = callback)
]

not_again = [
    lambda callback: player_one.set_busy(False, callback = callback)
]

myla_sequence = [
    lambda callback: player_one.set_busy(True, callback = callback),
    lambda callback: myla.turn_to_face(player_one, callback = callback),

    lambda callback: engine.show_dialogue_with_options(
        "Would you like the cutting script again?",
        {
            "Yes": lambda: engine.run_callback_list_sequence(again),
            "No" : lambda: engine.run_callback_list_sequence(not_again)
        }
    )

]

myla.player_action = lambda player_object: engine.run_callback_list_sequence(myla_sequence)

#global settings for level
#engine.play_music("eery")
engine.set_ui_colours((200,255,200),(215,255,215)) #TODO: save these colours in the config.
engine.set_py_tabs(9)

#initial setting of players
player_one.face_south()
player_one.focus()
myla.follow(player_one)

#beginning of level
engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("This is a dangerous forest, let me jump into your bag!", callback = callback),
        lambda callback: myla.move_east(callback = callback),
        lambda callback: player_one.face_east(callback = callback),
        lambda callback: myla.set_visible(False, callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback)
    ])

#what happens when you stand on a vine
def vine_die_action():
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("Aagh! The vines got you!", callback = callback)
    ])

#scattered vines about the place
vine0a.grow(player_one)
vine0a.contact_action = vine_die_action

vine0b.grow(player_one)
vine0b.contact_action = vine_die_action

vine0c.grow(player_one)
vine0c.contact_action = vine_die_action

vine4a.grow(player_one)
vine4a.contact_action = vine_die_action
vine4b.grow(player_one)
vine4b.contact_action = vine_die_action
vine4c.grow(player_one)
vine4c.contact_action = vine_die_action
vine4d.grow(player_one)
vine4d.contact_action = vine_die_action
vine4e.grow(player_one)
vine4e.contact_action = vine_die_action
vine4f.grow(player_one)
vine4f.contact_action = vine_die_action
vine4g.grow(player_one)
vine4g.contact_action = vine_die_action
vine4h.grow(player_one)
vine4h.contact_action = vine_die_action
vine4i.grow(player_one)
vine4i.contact_action = vine_die_action
vine4j.grow(player_one)
vine4j.contact_action = vine_die_action
vine4k.grow(player_one)
vine4k.contact_action = vine_die_action
vine4l.grow(player_one)
vine4l.contact_action = vine_die_action
vine4m.grow(player_one)
vine4m.contact_action = vine_die_action
vine4n.grow(player_one)
vine4n.contact_action = vine_die_action
vine4o.grow(player_one)
vine4o.contact_action = vine_die_action

#player meets vines 1
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

#player meets vines 2

cycle_length = 1.40

vine2a_list_sequence = [
    lambda callback: vine2a.wait(cycle_length, callback = callback),
    lambda callback: vine2a.grow(player_one, callback = callback),
    lambda callback: vine2a.wait(cycle_length, callback = callback),
    lambda callback: vine2a.destroy(callback = callback),
    lambda callback: engine.run_callback_list_sequence(vine2a_list_sequence)
]

vine2c_list_sequence = [
    lambda callback: vine2c.wait(cycle_length, callback = callback),
    lambda callback: vine2c.grow(player_one, callback = callback),
    lambda callback: vine2c.wait(cycle_length, callback = callback),
    lambda callback: vine2c.destroy(callback = callback),
    lambda callback: engine.run_callback_list_sequence(vine2c_list_sequence)
]

vine2b_list_sequence = [
    lambda callback: vine2b.grow(player_one, callback = callback),
    lambda callback: vine2b.wait(cycle_length, callback = callback),
    lambda callback: vine2b.destroy(callback = callback),
    lambda callback: vine2b.wait(cycle_length, callback = callback),
    lambda callback: engine.run_callback_list_sequence(vine2b_list_sequence)
]
 
vine2d_list_sequence = [
    lambda callback: vine2d.grow(player_one, callback = callback),
    lambda callback: vine2d.wait(cycle_length, callback = callback),
    lambda callback: vine2d.destroy(callback = callback),
    lambda callback: vine2d.wait(cycle_length, callback = callback),
    lambda callback: engine.run_callback_list_sequence(vine2d_list_sequence)
]

vine2a.contact_action = vine_die_action
vine2b.contact_action = vine_die_action
vine2c.contact_action = vine_die_action
vine2d.contact_action = vine_die_action

engine.run_callback_list_sequence(vine2a_list_sequence)
engine.run_callback_list_sequence(vine2b_list_sequence)
engine.run_callback_list_sequence(vine2c_list_sequence)
engine.run_callback_list_sequence(vine2d_list_sequence)

#player meets vines 3

alive_length = 0.80

vine3a_list_sequence = [
    lambda callback: vine3a.grow(player_one, callback = callback),
    lambda callback: vine3a.wait(5 * alive_length, callback = callback),
    lambda callback: vine3a.destroy(callback = callback),
    lambda callback: vine3a.wait(2 * alive_length, callback = callback),
    lambda callback: engine.run_callback_list_sequence(vine3a_list_sequence)
]

vine3b_list_sequence = [
    lambda callback: vine3b.grow(player_one, callback = callback),
    lambda callback: vine3b.wait(5 * alive_length, callback = callback),
    lambda callback: vine3b.destroy(callback = callback),
    lambda callback: vine3b.wait(2 * alive_length, callback = callback),
    lambda callback: engine.run_callback_list_sequence(vine3b_list_sequence)
]

vine3c_list_sequence = [
    lambda callback: vine3c.grow(player_one, callback = callback),
    lambda callback: vine3c.wait(5 * alive_length, callback = callback),
    lambda callback: vine3c.destroy(callback = callback),
    lambda callback: vine3c.wait(2 * alive_length, callback = callback),
    lambda callback: engine.run_callback_list_sequence(vine3c_list_sequence)
]

vine3d_list_sequence = [
    lambda callback: vine3d.grow(player_one, callback = callback),
    lambda callback: vine3d.wait(5 * alive_length, callback = callback),
    lambda callback: vine3d.destroy(callback = callback),
    lambda callback: vine3d.wait(2 * alive_length, callback = callback),
    lambda callback: engine.run_callback_list_sequence(vine3d_list_sequence)
]

vine3e_list_sequence = [
    lambda callback: vine3e.grow(player_one, callback = callback),
    lambda callback: vine3e.wait(5 * alive_length, callback = callback),
    lambda callback: vine3e.destroy(callback = callback),
    lambda callback: vine3e.wait(2 * alive_length, callback = callback),
    lambda callback: engine.run_callback_list_sequence(vine3e_list_sequence)
]

vine3f_list_sequence = [
    lambda callback: vine3f.grow(player_one, callback = callback),
    lambda callback: vine3f.wait(5 * alive_length, callback = callback),
    lambda callback: vine3f.destroy(callback = callback),
    lambda callback: vine3f.wait(2 * alive_length, callback = callback),
    lambda callback: engine.run_callback_list_sequence(vine3f_list_sequence)
]

vine3a.contact_action = vine_die_action
vine3b.contact_action = vine_die_action
vine3c.contact_action = vine_die_action
vine3d.contact_action = vine_die_action
vine3e.contact_action = vine_die_action
vine3f.contact_action = vine_die_action

vine3a.wait(0 * alive_length, lambda: engine.run_callback_list_sequence(vine3a_list_sequence))
vine3a.wait(1 * alive_length, lambda: engine.run_callback_list_sequence(vine3b_list_sequence))
vine3a.wait(2 * alive_length, lambda: engine.run_callback_list_sequence(vine3c_list_sequence))
vine3a.wait(3 * alive_length, lambda: engine.run_callback_list_sequence(vine3d_list_sequence))
vine3a.wait(4 * alive_length, lambda: engine.run_callback_list_sequence(vine3e_list_sequence))
vine3a.wait(5 * alive_length, lambda: engine.run_callback_list_sequence(vine3f_list_sequence))

#introducing the knife
knife1.set_cut_power(5)

met_knife = False
def meet_knife():
    global met_knife
    if not met_knife:
        met_knife = True
        knife_sequence = [
            lambda callback: player_one.set_busy(True, callback = callback),
            lambda callback: engine.show_dialogue("Ooh! Go to the knife and stand on it. Then you will be able to pick it up." , callback = callback),
            lambda callback: player_one.set_busy(False, callback = callback)
        ]
        engine.run_callback_list_sequence(knife_sequence)

trigger_knife1.player_walked_on = lambda player_object: meet_knife()

met_cut = False
def meet_cut():
    global met_cut

    if not met_cut:
        met_cut = True
        cut_sequence = [
            lambda callback: player_one.set_busy(True, callback = callback),
            lambda callback: engine.show_dialogue("Oh! I forgot to tell you how to use the knife!" , callback = callback),
            lambda callback: engine.show_dialogue("Use the function cut() in your PyScripter to cut down an object in front of you" , callback = callback),
            lambda callback: engine.clear_scripter(callback = callback),
            lambda callback: engine.insert_to_scripter("cut()", callback = callback),
            lambda callback: engine.show_dialogue("Once a vine has been cut down, it cannot grow back!" , callback = callback),
            lambda callback: player_one.set_busy(False, callback = callback)
        ]
        engine.run_callback_list_sequence(cut_sequence)

trigger_knife2a.player_walked_on = lambda player_object: meet_cut()
trigger_knife2b.player_walked_on = lambda player_object: meet_cut()

#global settings for level
#engine.play_music("eery")
engine.set_ui_colours((200,255,200),(215,255,215)) #TODO: save these colours in the config.
engine.set_py_tabs(9)

#initial setting of players
#player_one.face_south()
player_one.focus()
myla.follow(player_one)

#crocodile1.focus()
crocodile1.rand_explore()

vine_list = [
    vine0a,
    vine0b,
    vine0c,
    vine1,
    vine2a,
    vine2b,
    vine2c,
    vine2d,
    vine3a,
    vine3b,
    vine3c,
    vine3d,
    vine3e,
    vine3f,
    vine4a,
    vine4b,
    vine4c,
    vine4d,
    vine4e,
    vine4f,
    vine4g,
    vine4h,
    vine4i,
    vine4j,
    vine4k,
    vine4l,
    vine4n,
    vine4n,
    vine4o,
    vine51,
    vine52,
    vine53,
    vine54,
    vine55,
    vine56,
    vine57,
    vine58,
    vine59,
    vine61,
    vine62,
    vine63,
    vine64,
    vine65,
    vine66,
    vine67,
    vine68,
    vine69
]

whetstone1.prepare(5, vine_list)
whetstone2.prepare(5, vine_list)
whetstone3.prepare(7, vine_list)

#beginning of level
engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("This is a dangerous forest, let me jump into your bag!", callback = callback),
        lambda callback: myla.move_east(callback = callback),
        lambda callback: player_one.face_east(callback = callback),
        lambda callback: myla.set_visible(False, callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback)
    ])

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

trigger1a.player_walked_on = lambda player_object: meet_vine1()
trigger1b.player_walked_on = lambda player_object: meet_vine1()

#player meets vines 2

cycle_length = 1.40

vine2a_list_sequence = [
    lambda callback: vine2a.wait(cycle_length, callback = callback),
    lambda callback: vine2a.grow(callback = callback),
    lambda callback: vine2a.wait(cycle_length, callback = callback),
    lambda callback: vine2a.shrink(callback = callback),
    lambda callback: engine.run_callback_list_sequence(vine2a_list_sequence)
]

vine2c_list_sequence = [
    lambda callback: vine2c.wait(cycle_length, callback = callback),
    lambda callback: vine2c.grow(callback = callback),
    lambda callback: vine2c.wait(cycle_length, callback = callback),
    lambda callback: vine2c.shrink(callback = callback),
    lambda callback: engine.run_callback_list_sequence(vine2c_list_sequence)
]

vine2b_list_sequence = [
    lambda callback: vine2b.grow(callback = callback),
    lambda callback: vine2b.wait(cycle_length, callback = callback),
    lambda callback: vine2b.shrink(callback = callback),
    lambda callback: vine2b.wait(cycle_length, callback = callback),
    lambda callback: engine.run_callback_list_sequence(vine2b_list_sequence)
]
 
vine2d_list_sequence = [
    lambda callback: vine2d.grow(callback = callback),
    lambda callback: vine2d.wait(cycle_length, callback = callback),
    lambda callback: vine2d.shrink(callback = callback),
    lambda callback: vine2d.wait(cycle_length, callback = callback),
    lambda callback: engine.run_callback_list_sequence(vine2d_list_sequence)
]

cycle_length, engine.run_callback_list_sequence(vine2a_list_sequence)
cycle_length, engine.run_callback_list_sequence(vine2b_list_sequence)
cycle_length, engine.run_callback_list_sequence(vine2c_list_sequence)
cycle_length, engine.run_callback_list_sequence(vine2d_list_sequence)

#player meets vines 3

alive_length = 0.80

vine3a_list_sequence = [
    lambda callback: vine3a.grow(callback = callback),
    lambda callback: vine3a.wait(5 * alive_length, callback = callback),
    lambda callback: vine3a.shrink(callback = callback),
    lambda callback: vine3a.wait(2 * alive_length, callback = callback),
    lambda callback: engine.run_callback_list_sequence(vine3a_list_sequence)
]

vine3b_list_sequence = [
    lambda callback: vine3b.grow(callback = callback),
    lambda callback: vine3b.wait(5 * alive_length, callback = callback),
    lambda callback: vine3b.shrink(callback = callback),
    lambda callback: vine3b.wait(2 * alive_length, callback = callback),
    lambda callback: engine.run_callback_list_sequence(vine3b_list_sequence)
]

vine3c_list_sequence = [
    lambda callback: vine3c.grow(callback = callback),
    lambda callback: vine3c.wait(5 * alive_length, callback = callback),
    lambda callback: vine3c.shrink(callback = callback),
    lambda callback: vine3c.wait(2 * alive_length, callback = callback),
    lambda callback: engine.run_callback_list_sequence(vine3c_list_sequence)
]

vine3d_list_sequence = [
    lambda callback: vine3d.grow(callback = callback),
    lambda callback: vine3d.wait(5 * alive_length, callback = callback),
    lambda callback: vine3d.shrink(callback = callback),
    lambda callback: vine3d.wait(2 * alive_length, callback = callback),
    lambda callback: engine.run_callback_list_sequence(vine3d_list_sequence)
]

vine3e_list_sequence = [
    lambda callback: vine3e.grow(callback = callback),
    lambda callback: vine3e.wait(5 * alive_length, callback = callback),
    lambda callback: vine3e.shrink(callback = callback),
    lambda callback: vine3e.wait(2 * alive_length, callback = callback),
    lambda callback: engine.run_callback_list_sequence(vine3e_list_sequence)
]

vine3f_list_sequence = [
    lambda callback: vine3f.grow(callback = callback),
    lambda callback: vine3f.wait(5 * alive_length, callback = callback),
    lambda callback: vine3f.shrink(callback = callback),
    lambda callback: vine3f.wait(2 * alive_length, callback = callback),
    lambda callback: engine.run_callback_list_sequence(vine3f_list_sequence)
]

vine3a.wait(0 * alive_length, lambda: engine.run_callback_list_sequence(vine3a_list_sequence))
vine3a.wait(1 * alive_length, lambda: engine.run_callback_list_sequence(vine3b_list_sequence))
vine3a.wait(2 * alive_length, lambda: engine.run_callback_list_sequence(vine3c_list_sequence))
vine3a.wait(3 * alive_length, lambda: engine.run_callback_list_sequence(vine3d_list_sequence))
vine3a.wait(4 * alive_length, lambda: engine.run_callback_list_sequence(vine3e_list_sequence))
vine3a.wait(5 * alive_length, lambda: engine.run_callback_list_sequence(vine3f_list_sequence))

met_whetstone = False
def meet_whetstone():
    global met_whetstone
    if not met_whetstone:
        met_whetstone = True
        whetstone_sequence = [
            lambda callback: player_one.set_busy(True, callback = callback),
            lambda callback: engine.show_dialogue("Ooh! Go to the whetstone and interact with it. Then you will be able to sharpen your whetstone with it." , callback = callback),
            lambda callback: player_one.set_busy(False, callback = callback)
        ]
        engine.run_callback_list_sequence(whetstone_sequence)

trigger_whetstone1.player_walked_on = lambda player_object: meet_whetstone()

met_cut = False
def meet_cut():
    global met_cut

    if not met_cut:
        met_cut = True
        cut_sequence = [
            lambda callback: player_one.set_busy(True, callback = callback),
            lambda callback: engine.show_dialogue("Oh! I forgot to tell you how to use the whetstone!" , callback = callback),
            lambda callback: engine.show_dialogue("Use the function cut() in your PyScripter to cut down an object in front of you" , callback = callback),
            lambda callback: engine.clear_scripter(callback = callback),
            lambda callback: engine.insert_to_scripter("cut()", callback = callback),
            lambda callback: engine.show_dialogue("Once a vine has been cut down, it cannot grow back!" , callback = callback),
            lambda callback: player_one.set_busy(False, callback = callback)
        ]
        engine.run_callback_list_sequence(cut_sequence)

trigger_whetstone2a.player_walked_on = lambda player_object: meet_cut()
trigger_whetstone2b.player_walked_on = lambda player_object: meet_cut()


jumpingvines = [vine51, vine52, vine53, vine54, vine55, vine56, vine57, vine58, vine59]

def vine_jumping(v):
    engine.run_callback_list_sequence([
        lambda callback: v.wait(cycle_length, callback = callback),
        lambda callback: v.grow(callback = callback),
        lambda callback: v.wait(cycle_length, callback = callback),
        lambda callback: v.shrink(callback = callback),
        lambda callback: vine_jumping(v)
    ])

for vine in jumpingvines:
    vine_jumping(vine)

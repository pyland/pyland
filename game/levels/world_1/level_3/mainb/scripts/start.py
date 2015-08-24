player_one.focus()
engine.play_music("beach")

intro_dialogue = [
    lambda callback: player_one.set_busy(True, callback = callback),
    lambda callback: myla.move_west(callback = callback),
    lambda callback: myla.move_west(callback = callback),
    lambda callback: myla.move_west(callback = callback),
    lambda callback: myla.move_west(callback = callback),
    lambda callback: myla.move_west(callback = callback),
    lambda callback: myla.move_west(callback = callback),
    lambda callback: myla.move_west(callback = callback),
    lambda callback: player_one.face_east(callback = callback),
    lambda callback: engine.show_dialogue("Myla: We need to go west, but the boulder is blocking us!", callback = callback),
    lambda callback: engine.show_dialogue("Careful, those crocodiles they are next to you!", callback = callback),
    lambda callback: engine.show_dialogue("Since there is only one bridge, I'll just jump into your bag", callback =callback),
    lambda callback: player_one.face_west(callback = callback),
    lambda callback: myla.set_visible(False, callback = callback),
    lambda callback: myla.move_to((0,0), 0.0001, callback = callback),
    lambda callback: player_one.set_busy(False, callback =callback)
    ]

engine.run_callback_list_sequence(intro_dialogue)
b1_croc = [
croc_0,
croc_1,
croc_2,
croc_3,
croc_4,
croc_5,
croc_6,
croc_7,
croc_8,
croc_9]

for i in range(len(b1_croc)):
    croc = b1_croc[i]
    croc.change_state("swim")

    if i % 2 == 0:
        croc.face_east()
    else:
        croc.face_west()
    croc.move_horizontal(player_one)


b2_croc = [croc_b2_1,
croc_b2_2,
croc_b2_3,
croc_b2_4,
croc_b2_5,
croc_b2_6,
croc_b2_7,
croc_b2_8,
croc_b2_9,
croc_b2_10
]

for croc in [croc_b2_1, croc_b2_2, croc_b2_3, croc_b2_7,croc_b2_8,croc_b2_9,croc_b2_10]:
    croc.face_east()
    croc.change_state("swim")

for croc in [croc_b2_4,croc_b2_5,croc_b2_6]:
    croc.face_west()
    croc.change_state("swim")

trigger_croc = [trigger_croc_0,
trigger_croc_1,
trigger_croc_2,
trigger_croc_3,
trigger_croc_4,
trigger_croc_5,
trigger_croc_6,
trigger_croc_7,
trigger_croc_8,
trigger_croc_9,
trigger_croc_10]

def player_walked_on_ti():
    x,y = player_one.get_position()
    for a in [-1, 1]:
        for obj in engine.get_objects_at((x+a,y)):
            if obj in b2_croc:
                return obj.lose(player_one)
        for obj in engine.get_objects_at((x,y+a)):
            if obj in b2_croc:
                return obj.lose(player_one)

for t in trigger_croc:
    t.player_walked_on = lambda player_object: player_walked_on_ti()


first_time_a = True
first_time_b = True
def player_walked_on_challenge2a():
    global first_time_a;
    if first_time_a:
        first_time_a = False
        engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("Myla: Good job!", callback = callback),
        lambda callback: engine.show_dialogue("Those crocodiles are blocking our path...", callback = callback),
        lambda callback: engine.show_dialogue("I've put a script in your PyScripter called yell()", callback = callback),
        lambda callback: engine.clear_scripter(callback = callback),
        lambda callback: engine.insert_to_scripter("yell()", callback = callback),
        lambda callback: engine.show_dialogue("Face the crocodiles and run the script in order to aggrivate them", callback = callback),
        lambda callback: engine.show_dialogue("Make sure you get out of the way before he gets to you though!", callback = callback),
        lambda callback: player_one.set_busy(False, callback =callback),
        ])





def player_walked_on_challenge2b():
    global first_time_b;
    global myla_follow
    if first_time_b:
        first_time_b = False
        challenge2_dialogue = [
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("Myla: You did it!!", callback = callback),
        lambda callback: engine.show_dialogue("Let me out of your bag now!", callback = callback),
        lambda callback: player_one.set_busy(False, callback =callback),
        lambda callback: player_one.move_west(callback = callback),
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: myla.move_to((12,0), 0.0001, callback = callback),
        lambda callback: myla.set_visible(True, callback = callback),
        lambda callback: player_one.face_east(callback = callback),
        lambda callback: engine.show_dialogue("These crocodiles are making me nervous...lets get out of here", callback = callback),
        lambda callback: myla.follow(player_one, callback = callback),
        lambda callback: player_one.set_busy(False, callback =callback),
        ]

        engine.run_callback_list_sequence(challenge2_dialogue)



    
challenge2a.player_walked_on = lambda player_object: player_walked_on_challenge2a()
challenge2b.player_walked_on = lambda player_object: player_walked_on_challenge2b()

-
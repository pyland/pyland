player_one.focus()
croc_0.change_state("swim")
croc_1.change_state("swim")
croc_2.change_state("swim")
croc_3.change_state("swim")
croc_4.change_state("swim_dark")
croc_5.change_state("swim_lite")

croc_0.face_east()
croc_1.face_east()
croc_2.face_east()

croc = [
croc_0,
croc_1,
croc_2,
croc_3,
croc_4,
croc_5
]

croc_4.oscillate = 1
croc_5.oscillate = 2

croc[0].face_east()
croc[1].face_east()
croc[2].face_east()
croc[3].face_east()
croc[4].face_south()
croc[5].face_west()

triggers = [trigger_0,
trigger_1,
trigger_2,
trigger_3,
trigger_4,
trigger_5,
trigger_6,
trigger_7,
trigger_8,
trigger_9,
trigger_10,
trigger_11,
trigger_12,
trigger_13,
trigger_14,
trigger_15, 
trigger_16,
trigger_17,
trigger_18,
trigger_19,
trigger_20,
trigger_21,
trigger_22,
trigger_23,
trigger_24,
trigger_25,
trigger_26,
trigger_27,
trigger_28,
trigger_29]


def player_walked_on_ti():
    x,y = player_one.get_position()
    for a in [-1, 1]:
        for obj in engine.get_objects_at((x+a,y)):
            if obj in croc:
                return obj.lose(player_one)
        for obj in engine.get_objects_at((x,y+a)):
            if obj in croc:
                return obj.lose(player_one)

for t in triggers:
    t.player_walked_on = lambda player_object: player_walked_on_ti()







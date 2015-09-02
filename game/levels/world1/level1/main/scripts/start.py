#commence save-data set-up
world_name = "world1"
level_name = "level1"
map_name = "main"

engine.update_world_text("1")
engine.update_level_text("1")
engine.set_ui_colours((200,255,200),(215,255,215))

player_one.focus()
engine.play_music("beach")

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

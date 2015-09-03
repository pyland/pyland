#Heidi is with you going to find this secret device
#She's scared off by the crocodiles but you still go
#Pick it up and meet Myla


#commence save-data set-up
world_name = "world1"
level_name = "level1"
map_name = "main"

player_data.load(engine.get_player_name())
player_data.set_map(world_name, level_name = level_name, map_name = map_name) #change the map and save that the map has changed

end.player_walked_on = lambda player_object: player_data.save_and_exit("/world1")

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
    croc.killable = [player_one]
    croc.change_state("swim")

    if i % 2 == 0:
        croc.face_east()
    else:
        croc.face_west()
    croc.move_horizontal()

start_callback = [
        lambda callback: camera.focus(callback = callback),
        lambda callback: engine.show_dialogue("Attention Pyland Citizens: Use the arrow keys to move your player", callback = callback),
        lambda callback: player_one.focus(callback = callback),
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: heidi.turn_to_face(player_one, callback = callback),
        lambda callback: engine.show_dialogue("Heidi: So you think you there's a magical scripting device across the river huh?", callback = callback),
        lambda callback: engine.show_dialogue("Looks pretty dangerous but if you insist...", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback),
        lambda callback: heidi.start_turning(1.0, 7, callback = callback),
]



heidi_talk.player_walked_on = lambda player_object: engine.run_callback_list_sequence(start_callback)

heidi_callback = [
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: heidi.stop_turning(callback = callback),
        lambda callback: heidi.turn_to_face(player_one, callback = callback),
        lambda callback: engine.show_dialogue("You think you can make it across the bridge?!", callback = callback),
        lambda callback: heidi.start_turning(1.0, 7, callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback),
]



heidi.player_action  = lambda player_object: engine.run_callback_list_sequence(heidi_callback)

croc_callback = [
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: heidi.turn_to_face(player_one, callback = callback),
        lambda callback: engine.show_dialogue("Heidi: And I forgot to mention, don't stand next to any crocodiles...", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback),
]

croc_warning.player_walked_on = lambda player_object: engine.run_callback_list_sequence(croc_callback)

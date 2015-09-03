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
player_one.face_south()
player_one.set_cuts_left(3)
engine.play_music("world_1_jungle")

engine.run_callback_list_sequence([
	lambda callback: player_one.set_busy(True, callback = callback),
	lambda callback: engine.show_dialogue("This region is filled with many vines that one has to cut. One can use cut() to cut down vines.", callback = callback),
    lambda callback: engine.clear_scripter(callback = callback),
    lambda callback: engine.insert_to_scripter("#function used to cut down an object in front of the character\ncut()", callback = callback),
    lambda callback: player_one.set_busy(False, callback =callback)
])

def go_next(player_object):
    player_data.save_and_exit("/world1")

end_level.player_walked_on = go_next

def leave_beg(player_object):
    player_data.save_and_exit("/world1")

exit_level_start.player_walked_on = leave_beg

vine_list = [
    vine0,
    vine1,
    vine2
]

###Start actual content
def explain_cut(player_object):
	engine.run_callback_list_sequence([
		lambda callback: player_one.set_busy(True, callback = callback),
		lambda callback: engine.show_dialogue("Your knife gets blunt after cutting. Use get_cuts_left() to see how many cuts you have left.", callback = callback),
		lambda callback: engine.show_dialogue("Remember, you need to print the value that get_cuts_left() gives you to actually read it", callback = callback),
	    lambda callback: engine.clear_scripter(callback = callback),
	    lambda callback: engine.insert_to_scripter("#function used to cut down an object in front of the character\ncut()\n#function to get the number of cuts left\nget_cuts_left()\n#remember to print() the above value if you want to see it", callback = callback),
	    lambda callback: player_one.set_busy(False, callback =callback)
	])

explain_cut_trigger.player_walked_on = explain_cut

def explain_whetstone(player_object):
	engine.run_callback_list_sequence([
		lambda callback: player_one.set_busy(True, callback = callback),
		lambda callback: engine.show_dialogue("Go up to the whetsone ahead to sharpen your knife and get more cuts", callback = callback),
		lambda callback: engine.show_dialogue("Since sharpening knives takes long, vines grow back while you sharpen your knife", callback = callback),
	    lambda callback: player_one.set_busy(False, callback =callback)
	])

explain_whetstone_trigger.player_walked_on = explain_whetstone

whetstone1.prepare(2, vine_list)

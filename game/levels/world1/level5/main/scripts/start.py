from random import randint

#commence save-data set-up
world_name = "world1"
level_name = "level5"
map_name = "main"

engine.update_world_text("1")
engine.update_level_text("5")

player_data.load(engine.get_player_name())
player_data.set_map(world_name, level_name = level_name, map_name = map_name) #change the map and save that the map has changed
#end save-data set-up

engine.enable_py_scripter()
player_one.focus()


def go_next(player_object):
    player_data.save_and_exit("/world1/level6/main")

end_level.player_walked_on = go_next

def leave_beg(player_object):
    player_data.save_and_exit("/world1")

exit_level_start.player_walked_on = leave_beg

scroll1.set_message("The second boulder from the left is an illusion.")
scroll2.set_message("If the other scroll has an odd number,\nthe middle tree stumps in the north are fake.\nElse, the tree stumps on the right are fake.")

rand_block = randint(0,999)

scroll3.set_message("Number: " + str(rand_block))

def do_nothing(player_object):
	pass

block1.player_walked_on = do_nothing
block2.player_walked_on = do_nothing

if(rand_block % 2 == 0):
	block1.set_solidity(True)
else:
	block2.set_solidity(True)

###### Top chamber
centre1.player_walked_on = do_nothing

#start from topright and go clockwise

count1 = [0, 0, 0, 0]

door1.player_walked_on = do_nothing
door1.set_solidity(True)

ans1 = [1, 1, 1, 1]

rand_block = randint(1,16)
magic_message = ""

def add_magic_message():
	global rand_block
	global magic_message
	if(rand_block % 2 == 0):
		magic_message += " odd number of times\n"
		rand_block //= 2
		return 1
	else:
		magic_message += "even number of times\n"
		rand_block //= 2
		return 0

magic_message += "Top-right grass tile: "
ans1[0] = add_magic_message()
magic_message += "Bottom-right grass tile: "
ans1[1] = add_magic_message()
magic_message += "Bottom-left grass tile: "
ans1[2] = add_magic_message()
magic_message += "Top-left grass tile: "
ans1[3] = add_magic_message()

scroll1a.set_message("The man up ahead is a wizard and he must be\non the centre grass tile for his magic to work.")
scroll1b.set_message("Once the wizard starts his magic, you need\nto step on as many grass tiles as indicated on the next scroll,\nafter stepping on at least one tile")
scroll1c.set_message(magic_message)
scroll1d.set_message("The magic the wizard performs makes the \nnorth-most log of wood on the east wall walkable")

tiles1 = [topright1, bottomright1, bottomleft1, topleft1]

def check():
	for i in range(4):
		if(ans1[i]%2 != count1[i]%2):
			door1.set_solidity(True)
			return
	door1.set_solidity(False)

def add(i):
	if pete.get_position() == centre1.get_position():
		count1[i] += 1
		check()

tiles1[0].player_walked_on = lambda player_object: add(0)
tiles1[1].player_walked_on = lambda player_object: add(1)
tiles1[2].player_walked_on = lambda player_object: add(2)
tiles1[3].player_walked_on = lambda player_object: add(3)

def pete_speak():
	if(pete.get_position() == centre1.get_position()):
		engine.run_callback_list_sequence([
			lambda callback: player_one.set_busy(True, callback = callback),
	        lambda callback: pete.turn_to_face(player_one, callback = callback),
	        lambda callback: engine.show_dialogue("I am performing my magic and you can harness my energy", callback = callback),
	        lambda callback: engine.show_dialogue("You have walked these many times on the magic tiles:", callback = callback),
	        lambda callback: engine.show_dialogue("Top-right:"+str(count1[0])+", Bottom-right:"+str(count1[1])+", Bottom-left:"+str(count1[2])+", Top-left:"+str(count1[3]), callback = callback),
	        lambda callback: player_one.set_busy(False, callback =callback)
		])
	else:
		engine.run_callback_list_sequence([
        lambda callback: pete.set_busy(False, callback = callback),
        lambda callback: engine.show_external_script(
            confirm_callback = lambda: engine.run_callback_list_sequence([
                lambda callback: player_one.set_busy(False, callback = callback),
                lambda callback: pete.run_script(script_to_run = 10)]),
            cancel_callback = lambda: player_one.set_busy(False),
            external_dialogue = "You can edit my script and I will run it!",
            script_init = lambda: engine.insert_to_scripter(""),
            character_object = pete
        )
    ])

pete.player_action = lambda player_object: pete_speak()

###### Right chamber
centre2.player_walked_on = do_nothing

#start from topright and go clockwise

count2 = [0, 0, 0, 0]

door2.player_walked_on = do_nothing
door2.set_solidity(True)

ans2 = [1, 1, 1, 1]

rand_block = randint(1,16)
magic_message = ""

magic_message += "Top-right grass tile: "
ans2[0] = add_magic_message()
magic_message += "Bottom-right grass tile: "
ans2[1] = add_magic_message()
magic_message += "Bottom-left grass tile: "
ans2[2] = add_magic_message()
magic_message += "Top-left grass tile: "
ans2[3] = add_magic_message()

scroll2a.set_message("The lady up ahead is a witch and she must be\non the centre grass tile for her magic to work.")
scroll2b.set_message("Once the witch starts her magic, you need\nto step on as many grass tiles as indicated on the next scroll,\nafter stepping on at least one tile")
scroll2c.set_message(magic_message)
scroll2d.set_message("The magic the witch performs makes the \nwest-most log of wood on the norht wall walkable")

tiles2 = [topright2, bottomright2, bottomleft2, topleft2]

def check():
	for i in range(4):
		if(ans2[i]%2 != count2[i]%2):
			door2.set_solidity(True)
			return
	door2.set_solidity(False)

def add(i):
	if maddie.get_position() == centre2.get_position():
		count2[i] += 1
		check()

tiles2[0].player_walked_on = lambda player_object: add(0)
tiles2[1].player_walked_on = lambda player_object: add(1)
tiles2[2].player_walked_on = lambda player_object: add(2)
tiles2[3].player_walked_on = lambda player_object: add(3)

def maddie_speak():
	if(maddie.get_position() == centre2.get_position()):
		engine.run_callback_list_sequence([
			lambda callback: player_one.set_busy(True, callback = callback),
	        lambda callback: maddie.turn_to_face(player_one, callback = callback),
	        lambda callback: engine.show_dialogue("I am performing my magic and you can harness my energy", callback = callback),
	        lambda callback: engine.show_dialogue("You have walked these many times on the magic tiles:", callback = callback),
	        lambda callback: engine.show_dialogue("Top-right:"+str(count2[0])+", Bottom-right:"+str(count2[1])+", Bottom-left:"+str(count2[2])+", Top-left:"+str(count2[3]), callback = callback),
	        lambda callback: player_one.set_busy(False, callback =callback)
		])
	else:
		engine.run_callback_list_sequence([
        lambda callback: maddie.set_busy(False, callback = callback),
        lambda callback: engine.show_external_script(
            confirm_callback = lambda: engine.run_callback_list_sequence([
                lambda callback: player_one.set_busy(False, callback = callback),
                lambda callback: maddie.run_script(script_to_run = 10)]),
            cancel_callback = lambda: player_one.set_busy(False),
            external_dialogue = "You can edit my script and I will run it!",
            script_init = lambda: engine.insert_to_scripter(""),
            character_object = maddie
        )
    ])

maddie.player_action = lambda player_object: maddie_speak()

hor_crocs = [croc1, croc3, croc5, croc7, croc9]
ver_crocs = [croc2, croc4, croc6, croc8]
for croc in hor_crocs:
	croc.check_kill([player_one])
	croc.move_horizontal()

for croc in ver_crocs:
	croc.check_kill([player_one])
	croc.move_vertical()
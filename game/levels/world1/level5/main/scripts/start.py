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

player_one.focus()


def go_next(player_object):
    player_data.save_and_exit("/world1/level6/main")

end_level.player_walked_on = go_next

def leave_beg(player_object):
    player_data.save_and_exit("/world1")

exit_level_start.player_walked_on = leave_beg

scroll1.set_message("The second boulder from the left is an illusion.")
scroll2.set_message("If the other scroll has an odd number,\nthe second tree stumps in the north are fake.\nElse, the tree stumps on the right are fake.")

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
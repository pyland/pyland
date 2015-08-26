#commence save-data set-up
level_name = "level_1"
world_name = "world_1"
map_name = "road_one"

player_data.load(engine.get_player_name())
player_data.set_map(world_name, level_name = level_name, map_name = map_name) #change the map and save that the map has changed
#end save-data set-up

#defining the possible states of the level
heidi_state_speaking_about_prank = 0 #not used on this map!
heidi_state_getting_prank_materials = 1
heidi_state_got_prank_materials = 2
#end state definitions


player_start_pos = (0, 0)
#setting the player's starting position
if player_data.get_previous_exit() == "/world_1/level_1/rossumberg/top":
    player_start_pos = exit_to_village_top.get_position()
    player_one.move_to(player_start_pos, callback = player_one.move_east)
elif player_data.get_previous_exit() == "/world_1/level_1/rossumberg/bottom":
    player_start_pos = exit_to_village_bottom.get_position()
    player_one.move_to(player_start_pos, callback = player_one.move_east)
# end settin the default starting position

#setting up the level's exits
def go_to_village_top(player_object):
    player_data.save_and_exit("/world_1/level_1/rossumberg", info = "top")

def go_to_village_bottom(player_object):
    player_data.save_and_exit("/world_1/level_1/rossumberg", info = "bottom")

def go_to_merchant(player_object):
    player_data.save_and_exit("/world_1/level_1/merchant")

def go_to_world(player_object):
    player_data.complete_level_and_save()
    player_data.save_and_exit("/world_1")

exit_to_village_top.player_walked_on = go_to_village_top
exit_to_village_bottom.player_walked_on = go_to_village_bottom
exit_to_merchant.player_walked_on = go_to_merchant
exit_to_world.player_walked_on = go_to_world

#end setting up the level exits

# general object set-up

village_sign.set_message("(west) rossumberg (north) bog (north) merchant")
bog_sign.set_message("(south) rossumberg (east) bog (north) merchant")
merchant_sign.set_message("(south) rossumberg (south) bog (north) merchant")

def no_potion_player_action(player_object):
    player_one.set_busy(True)
    engine.show_dialogue("The totems are cursed...", callback = lambda: player_one.set_busy(False))

no_potion.player_walked_on = lambda: None
no_potion.player_action = no_potion_player_action


def merchant_player_action(player_object):
        engine.run_callback_list_sequence([
            lambda callback: player_one.set_busy(True, callback = callback),
            lambda callback: engine.show_dialogue("Merchant: Sigh, I have nothing to sell anymore, who would even want bridges???", callback = callback),
            lambda callback: player_one.set_busy(False, callback = callback)
        ])

merchant.player_action = merchant_player_action
merchant.face_south()

def merchant_blocker_player_action(player_object):
    player_one.set_busy(True)
    engine.show_dialogue("Sorry, this part of the game is still in progress, please come back later!", callback = lambda: player_one.set_busy(False))

merchant_blocker.player_action = merchant_blocker_player_action
merchant_blocker.face_south()

# end general object set-up

#state-based set-up
heidi_state = player_data.get_level_state("heidi_state")
if heidi_state == heidi_state_getting_prank_materials:
    def heidi_player_action(player_object):
        player_one.set_busy(True)
        engine.show_dialogue("The merchant is North of here, let's head there!", callback = lambda: player_one.set_busy(False))

    heidi.player_action = heidi_player_action

    def annoying_croc_player_action(player_object):
        engine.run_callback_list_sequence([
            lambda callback: player_one.set_busy(True, callback = callback),
            lambda callback: engine.show_dialogue("Oi, no one is entering or leaving the bog. Tell us if you see a furry person walking around you punk!", callback = callback),
            lambda callback: player_one.wait(0.3, callback = callback),
            lambda callback: player_one.face_west(callback = callback),
            lambda callback: player_one.wait(0.3, callback = callback),
            lambda callback: engine.show_dialogue("Heidi: Oh look, a crocodile being a bully, what a suprise. Common let's head to the merchant North of here first, we can get the bogwater later.", callback = callback),
            lambda callback: player_one.set_busy(False, callback = callback)
        ])

    annoying_croc.player_action = annoying_croc_player_action
    annoying_croc.face_west()

    def merchant_player_action_stuff(player_object):
        engine.run_callback_list_sequence([
            lambda callback: player_one.set_busy(True, callback = callback),
            lambda callback: engine.show_dialogue("Merchant: Sigh, they haven't even properly completed my shop yet...", callback = callback),
            lambda callback: engine.show_dialogue("Look at this, they have given me these placeholder bridge tiles instead of my own wares!", callback = callback),
            lambda callback: engine.show_dialogue("Oh well, here is the stuff you ordered. I have no idea what you want with all this stuff, but who am I to judge?", callback = callback),
            lambda callback: player_one.wait(0.3, callback = callback),
            lambda callback: player_one.face_south(callback = callback),
            lambda callback: player_one.wait(0.3, callback = callback),
            lambda callback: engine.show_dialogue("Heidi: I'll go and set-up all this stuff back in the village! Why don't you head east to the bog? Hopefully the annoying crocodile has moved!", callback = callback),
            lambda callback: heidi.stop_follow(callback = callback),		
            lambda callback: heidi.move_south(callback = callback),
            lambda callback: heidi.move_south(callback = callback),
            lambda callback: heidi.move_east(callback = callback),
            lambda callback: heidi.move_east(callback = callback),
            lambda callback: heidi.move_east(callback = callback),
            lambda callback: heidi.move_south(callback = callback),
            lambda callback: heidi.move_south(callback = callback),
            lambda callback: heidi.set_solidity(False, callback = callback),
            lambda callback: heidi.set_visible(False, callback = callback),
            lambda callback: annoying_croc.set_solidity(False, callback = callback),
            lambda callback: annoying_croc.set_visible(False, callback = callback),
            lambda callback: player_one.set_busy(False, callback = callback)
        ], save_got_prank_materials)

    def save_got_prank_materials():
        merchant.player_action = merchant_player_action
        player_data.set_level_state("heidi_state", heidi_state_got_prank_materials)
        player_data.save()

    merchant.player_action = merchant_player_action_stuff

    heidi.set_solidity(False)
    heidi.wait(0.3, callback = lambda: heidi.move_to(player_start_pos, callback = lambda: heidi.follow(player_one)))
else:
    annoying_croc.set_solidity(False)
    annoying_croc.set_visible(False)
    heidi.set_solidity(False)
    heidi.set_visible(False)

# end object set-up
engine.play_music("world_1_jungle")
player_one.focus()



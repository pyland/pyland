#commence save-data set-up
level_name = "level1"
world_name = "world1"
map_name = "bridge"

engine.update_world_text("1")
engine.update_level_text("1")

player_data.load(engine.get_player_name())
player_data.set_map(world_name, level_name = level_name, map_name = map_name) #change the map and save that the map has changed
#end save-data set-up

#defining the possible states of the level
heidi_state_warn_about_crocodiles = 0
heidi_state_following_player = 1
heidi_state_stand_by_bridge = 2
#end state definitions


player_start_pos = (0, 0)
#setting the player's starting position
if player_data.get_previous_exit() == "/world1/level1/rossumberg/west":
    player_start_pos = exit_to_village_west.get_position()
    player_one.move_to(player_start_pos, callback = player_one.move_north)
elif player_data.get_previous_exit() == "/world1/level1/rossumberg/east":
    player_start_pos = exit_to_village_east.get_position()
    player_one.move_to(player_start_pos, callback = player_one.move_north)
# end settin the default starting position

#setting up the level's exits
def go_to_village_west(player_object):
    player_data.save_and_exit("/world1/level1/rossumberg", info = "west")

def go_to_village_east(player_object):
    player_data.save_and_exit("/world1/level1/rossumberg", info = "east")

def go_to_world(player_object):
    player_data.complete_level_and_save()
    player_data.save_and_exit("/world1/level1/bridge_end")

exit_to_village_west.player_walked_on = go_to_village_west
exit_to_village_east.player_walked_on = go_to_village_east
exit_to_world.player_walked_on = go_to_world

#end setting up the level exits

# general object set-up

village_sign.set_message("(west) rossumberg (north) bog (north) merchant")
bog_sign.set_message("(south) rossumberg (east) bog (north) merchant")

def no_potion_player_action(player_object):
    player_one.set_busy(True)
    engine.show_dialogue("The totems are cursed...", callback = lambda: player_one.set_busy(False))

no_potion.player_walked_on = lambda: None
no_potion.player_action = no_potion_player_action
# end general object set-up

#state-based set-up
heidi_state = player_data.get_level_state("heidi_state")
if heidi_state == heidi_state_following_player:
    def heidi_player_action(player_object):
        player_one.set_busy(True)
        engine.show_dialogue("This is bad, normally the crocodiles never come out this far.", callback = lambda: player_one.set_busy(False))

    heidi.player_action = heidi_player_action
    heidi.set_solidity(False)
    heidi.wait(0.3, callback = lambda: heidi.move_to(player_start_pos, callback = lambda: heidi.follow(player_one)))
else:
    heidi.set_solidity(False)
    heidi.set_visible(False)

# end object set-up
engine.play_music("world_1_jungle")
player_one.focus()



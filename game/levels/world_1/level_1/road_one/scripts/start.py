#commence set-up
player_name = engine.get_player_name()
player_data = engine.get_player_data(player_name)
current_level = "/world_1/level_1/road_one"

player_data["current_level"] = current_level
# Add the basic level data to the save file if it doesn't exist
if not (current_level in player_data["level_states"]):
    player_data["level_states"][current_level] = {}

#A simple variable to use to refer to level state
level_state = player_data["level_states"][current_level]

engine.save_player_data(player_name, player_data)

village_sign.set_message("(west) rossumberg (north) bog (north) merchant")
bog_sign.set_message("(south) rossumberg (east) bog (north) merchant")
merchant_sign.set_message("(south) rossumberg (south) bog (north) merchant")

def heidi_player_action(player_object):
    player_one.set_busy(True)
    engine.show_dialogue("The merchant is North of here, let's head there!", callback = lambda: player_one.set_busy(False))

heidi.player_action = heidi_player_action

def no_potion_player_action(player_object):
    player_one.set_busy(True)
    engine.show_dialogue("You aren't going to find any potions here m8.", callback = lambda: player_one.set_busy(False))

no_potion.player_walked_on = lambda: None
no_potion.player_action = no_potion_player_action

def go_to_village(player_object):
    engine.change_map("/world_1/level_1/rossumberg")

exit_to_village_top.player_walked_on = go_to_village
exit_to_village_bottom.player_walked_on = go_to_village

def annoying_croc_player_action(player_object):
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("Oi, no one is entering or leaving the bog. Tell us if you see a furry person walking around you punk!", callback = callback),
        lambda callback: player_one.wait(0.3, callback = callback),
        lambda callback: player_one.face_west(callback = callback),
        lambda callback: player_one.wait(0.3, callback = callback),
        lambda callback: engine.show_dialogue("What's up with that weirdo? Common let's head to the merchant North of here first, we can get the bogwater later.", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback)
    ])

player_one.focus()

annoying_croc.player_action = annoying_croc_player_action
annoying_croc.face_west()

player_start_pos = player_one.get_position()
player_one.move_east(callback = lambda: heidi.move_to(player_start_pos, callback = lambda: heidi.follow(player_one)))

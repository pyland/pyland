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
if player_data.previous_exit_is("world1", level_name = "level1", map_name = "rossumberg", info = "west"):
    player_start_pos = exit_to_village_west.get_position()
    player_one.move_to(player_start_pos, callback = player_one.move_north)
elif player_data.previous_exit_is("world1", level_name = "level1", map_name = "rossumberg", info = "east"):
    player_start_pos = exit_to_village_east.get_position()
    player_one.move_to(player_start_pos, callback = player_one.move_north)
elif player_data.previous_exit_is("world1", level_name = "level1", map_name = "bridge_end"):
    player_start_pos = exit_to_bridge_end.get_position()
    player_one.move_to(player_start_pos, callback = player_one.move_west)
# end settin the default starting position

#setting up the level's exits
def go_to_village_west(player_object):
    player_data.save_and_exit("/world1/level1/rossumberg", info = "west")

def go_to_village_east(player_object):
    player_data.save_and_exit("/world1/level1/rossumberg", info = "east")

def go_to_bridge_end(player_object):
    player_data.save_and_exit("/world1/level1/bridge_end")

exit_to_village_west.player_walked_on = go_to_village_west
exit_to_village_east.player_walked_on = go_to_village_east
exit_to_bridge_end.player_walked_on = go_to_bridge_end

#end setting up the level exits

# general object set-up

village_sign.set_message("You can pause the game at any-time by pressing the button in the top-left hand corner of the screen. \nSouth: Rossumberg\nEast: Bog")
bog_sign.set_message("Remember, you can use WASD to move as well as using the arrow keys. \nSouth:Rossumberg\nEast: Bog")

def no_potion_player_action(player_object):
    player_one.set_busy(True)
    engine.show_dialogue("The totems are cursed...", callback = lambda: player_one.set_busy(False))

no_potion.player_walked_on = lambda: None
no_potion.player_action = no_potion_player_action
# end general object set-up

#state-based set-up
heidi_state = player_data.get_level_state("heidi_state")
if heidi_state == heidi_state_following_player:

    def save_heidi_spoken_about_prank():
        player_data.set_level_state("heidi_state", heidi_state_stand_by_bridge)
        player_data.save()

    def heidi_chicken_out_action(player_object):
        engine.run_callback_list_sequence([
            lambda callback: player_one.set_busy(True, callback = callback),
            lambda callback: player_one.face_south(callback = callback),
            lambda callback: engine.show_dialogue("Heidi: Maybe I will just back out on this one, crocodiles are scary.", callback = callback),
            lambda callback: engine.show_dialogue("If a crocodiles touches you, you're a gonner, so I will hang back and give you moral support!", callback = callback),
            lambda callback: heidi.stop_follow(callback = callback),
            lambda callback: heidi.set_solidity(True, callback = callback),
            lambda callback: heidi.move_south(callback = callback),
            lambda callback: heidi.move_west(callback = callback),
            lambda callback: heidi.face_east(callback = callback),
            lambda callback: player_one.set_busy(False, callback = callback), 
        ], save_heidi_spoken_about_prank)

    heidi_chicken_out.player_walked_on = heidi_chicken_out_action
    heidi.set_solidity(False)
    heidi.wait(0.3, callback = lambda: heidi.move_to(player_start_pos, callback = lambda: heidi.follow(player_one)))
else:
    heidi.set_solidity(True)
    heidi.move_to((8,4))
    heidi.face_east()


def heidi_player_action(player_object):
    player_one.set_busy(True)
    engine.show_dialogue("This is bad, normally the crocodiles never come out this far.", callback = lambda: player_one.set_busy(False))

def heidi_totem_warning_action(player_object):
        engine.run_callback_list_sequence([
            lambda callback: player_one.set_busy(True, callback = callback),
            lambda callback: player_one.face_south(callback = callback),
            lambda callback: engine.show_dialogue("Heidi: " + engine.get_player_name() + "!!! Don't go that way, the crocodiles are just sitting there waiting to eat you!!!", callback = callback),
            lambda callback: player_one.set_busy(False, callback = callback), 
        ])

heidi.player_action = heidi_player_action
heidi_totem_warning.player_walked_on = heidi_totem_warning_action

# end object set-up
engine.play_music("world_1_jungle")
player_one.focus()
croc_0.face_south()
croc_1.move_vertical()
croc_2.move_horizontal()
croc_3.move_horizontal()
croc_4.move_horizontal()
croc_5.face_north()



#commence save-data set-up
level_name = "level1"
world_name = "world1"
map_name = "rossumberg"

engine.update_world_text("1")
engine.update_level_text("1")

player_data.load(engine.get_player_name())
player_data.set_map(world_name, level_name = level_name, map_name = map_name) #change the map and save that the map has changed
#end save-data set-up

#defining the possible states of the level
heidi_state_speaking_about_prank = 0
heidi_state_getting_prank_materials = 1
heidi_state_got_prank_materials = 2
#end state definitions


player_start_pos = (0, 0)
#setting the player's starting position
if player_data.get_previous_exit() == "/world1/level1/player_house":
    player_start_pos = exit_to_house.get_position()
    player_one.move_to(exit_to_house.get_position(), callback = player_one.move_south)
elif player_data.get_previous_exit() == "/world_1/level_1/road_one/top":
    player_start_pos = exit_to_road_top.get_position()
    player_one.move_to(exit_to_road_top.get_position(), callback = player_one.move_west)
elif player_data.get_previous_exit() == "/world_1/level_1/road_one/bottom":
    player_start_pos = exit_to_road_bottom.get_position()
    player_one.move_to(exit_to_road_bottom.get_position(), callback = player_one.move_west)
else:
    player_one.move_to(exit_to_house.get_position(), callback = player_one.move_south)
# end settin the default starting position

#setting up the level's exits
def go_to_house(player_object):
    player_data.save_and_exit("/world1/level1/player_house")

def go_to_road_top(player_object):
    player_data.save_and_exit("/world1/level1/bridge", info = "top")

def go_to_road_bottom(player_object):
    player_data.save_and_exit("/world1/level1/bridge", info = "bottom")

exit_to_house.player_walked_on = go_to_house
exit_to_road_top.player_walked_on = go_to_road_top
exit_to_road_bottom.player_walked_on = go_to_road_bottom

# end setting up the level's exits

def heidi_player_action(player_object):
    player_one.set_busy(True)
    engine.show_dialogue("Remember " + engine.get_player_name() + ", the merchant is East.", callback = lambda: player_one.set_busy(False))

heidi.player_action = heidi_player_action

route_sign.set_message("Rossumberg, a peaceful town. \n(east) merchant (east) bog")

""" LEVEL EVENTS """

""" Only have heidi speak to player about the prank if they haven't heard about it yet! """
heidi_state = player_data.get_level_state("heidi_state")
if heidi_state == heidi_state_speaking_about_prank:
    """ Heidi introducing herself and walking over to the player """
    heidi_introduction_sequence = [
        lambda callback: heidi.move_to(exit_to_road_top.get_position(), callback = callback),
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: heidi.move_south(callback = callback),
        lambda callback: heidi.move_south(callback = callback),
        lambda callback: heidi.move_south(callback = callback),
        lambda callback: heidi.move_south(callback = callback),
        lambda callback: heidi.move_south(callback = callback),
        lambda callback: heidi.move_south(callback = callback),
        lambda callback: heidi.move_south(callback = callback),
        lambda callback: heidi.face_east(callback = callback),
        lambda callback: player_one.face_west(callback = callback),
        lambda callback: engine.show_dialogue(engine.get_player_name() + "!", callback = callback),
        lambda callback: heidi.move_east(callback = callback),
        lambda callback: heidi.move_east(callback = callback),
        lambda callback: heidi.move_east(callback = callback),
        lambda callback: heidi.move_east(callback = callback),
        lambda callback: heidi.move_east(callback = callback),
        lambda callback: engine.show_dialogue("There was a huge storm and a surge of lightning last night that broke our PyRunners.", callback = callback),
        lambda callback: engine.show_dialogue("Crocodiles are attacking our village bridge to the north of here!", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback),
        lambda callback: heidi.follow(player_one, callback = callback)
    ]

    engine.run_callback_list_sequence(heidi_introduction_sequence)

elif heidi_state == heidi_state_getting_prank_materials:
    heidi.set_solidity(False)
    heidi.wait(0.3, callback = lambda: heidi.move_to(player_start_pos, callback = lambda: heidi.follow(player_one)))

""" end of everything too with heidi asking about the prank """

player_one.focus()

engine.play_music("calm")
engine.set_ui_colours((200,255,200),(215,255,215)) #TODO: save these colours in the config.

villager1.set_dialogue_list(["Oh " + engine.get_player_name() + ", not up to mischief again are you?"])
villager2.set_dialogue_list(["These PyRunners that they make nowadays are great.", "If only I could write my own scripts instead of just running the ones they give me..."])
villager3.set_dialogue_list(["I like turtles."])
villager4.set_dialogue_list(["Ok."])

villager1.rand_explore()
villager2.rand_explore()
villager3.rand_explore()
villager4.rand_explore()

#engine.disable_py_scripter()
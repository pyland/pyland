#commence save-data set-up
level_name = "level_1"
world_name = "world_1"
map_name = "rossumberg"
current_map = "/" + world_name + "/" + level_name + "/" + map_name

player_name = engine.get_player_name()
player_data = engine.get_player_data(player_name)
player_data["current_map"] = current_map
level_data = player_data["level_data"][world_name][level_name]

engine.save_player_data(player_name, player_data)
#end save-data set-up

#defining the possible states of the level
heidi_state_speaking_about_prank = 0
heidi_state_getting_prank_materials = 1
#end state definitions


player_start_pos = (0, 0)
#setting the player's starting position
if player_data["previous_exit"] == "/world_1/level_1/player_house":
    player_start_pos = exit_to_house.get_position()
    player_one.move_to(exit_to_house.get_position(), callback = player_one.move_south)
elif player_data["previous_exit"] == "/world_1/level_1/road_one/top":
    player_start_pos = exit_to_road_top.get_position()
    player_one.move_to(exit_to_road_top.get_position(), callback = player_one.move_west)
elif player_data["previous_exit"] == "/world_1/level_1/road_one/bottom":
    player_start_pos = exit_to_road_bottom.get_position()
    player_one.move_to(exit_to_road_bottom.get_position(), callback = player_one.move_west)
else:
    player_one.move_to(exit_to_house.get_position(), callback = player_one.move_south)
# end settin the default starting position

#setting up the level's exits
def go_to_house(player_object):
    player_data["previous_exit"] = current_map
    engine.save_player_data(player_name, player_data)
    engine.change_map("/world_1/level_1/player_house")

def go_to_road_top(player_object):
    player_data["previous_exit"] = current_map + "/top"
    engine.save_player_data(player_name, player_data)
    engine.change_map("/world_1/level_1/road_one")

def go_to_road_bottom(player_object):
    player_data["previous_exit"] = current_map + "/bottom"
    engine.save_player_data(player_name, player_data)
    engine.change_map("/world_1/level_1/road_one")

exit_to_house.player_walked_on = go_to_house
exit_to_road_top.player_walked_on = go_to_road_top
exit_to_road_bottom.player_walked_on = go_to_road_bottom

# end setting up the level's exits

def heidi_player_action(player_object):
    player_one.set_busy(True)
    engine.show_dialogue("Remember, the merchant is East.", callback = lambda: player_one.set_busy(False))

heidi.player_action = heidi_player_action

route_sign.set_message("Rossumberg, where your mum is the village bicycle \n(east) merchant (east) bog")

""" LEVEL EVENTS """

""" Only have heidi speak to player about the prank if they haven't heard about it yet! """
if level_data["heidi_state"] == heidi_state_speaking_about_prank:
    """ Heidi introducing herself and walking over to the player """
    heidi_introduction_sequence = [
        lambda callback: heidi.move_to(exit_to_road_top.get_position(), callback = callback),
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: player_one.face_south(callback = callback),
        lambda callback: heidi.face_west(callback = callback),
        lambda callback: engine.show_dialogue(engine.get_dialogue(current_map, "heidi_shout_player_name", {"player_name": player_name}), callback = callback),
        lambda callback: heidi.move_west(callback = callback),
        lambda callback: heidi.move_west(callback = callback),
        lambda callback: heidi.move_west(callback = callback),
        lambda callback: heidi.move_west(callback = callback),
        lambda callback: heidi.move_west(callback = callback),
        lambda callback: heidi.move_north(callback = callback),
        lambda callback: heidi.move_west(callback = callback),
        lambda callback: player_one.face_east(callback = callback),
    ]

    """ Run the introdoction dialogue, and end it with asking the player about wether they know about the "big day today", (the prank), and if they say yes, asking what they think it is."""
    engine.run_callback_list_sequence(
        heidi_introduction_sequence,
        lambda: engine.show_dialogue_with_options(
            engine.get_dialogue(current_map, "heidi_today_is_big_day"),
            {
                engine.get_dialogue("shared", "yes"): lambda: engine.show_dialogue_with_options(
                    engine.get_dialogue(current_map, "heidi_question_player_about_day"),
                    {
                        "Ummmmm..."  : lambda: engine.run_callback_list_sequence(heidi_explain_prank_sequence, callback = save_heidi_spoken_about_prank),
                        "Ahhhhhhh...": lambda: engine.run_callback_list_sequence(heidi_explain_prank_sequence, callback = save_heidi_spoken_about_prank)
                    }
                ),
                engine.get_dialogue("shared", "no") : lambda: engine.run_callback_list_sequence(heidi_explain_prank_sequence, callback = save_heidi_spoken_about_prank)
            }
        )
    )

    def save_heidi_spoken_about_prank():
        level_data["heidi_state"] = heidi_state_getting_prank_materials
        engine.save_player_data(player_name, player_data)

    """ Heidi explains to the player that today is the day of the big prank, and that they need to head to the merchant for materials """
    heidi_explain_prank_sequence = [
        lambda callback: engine.show_dialogue("You forgot you goon!", callback = callback),
        lambda callback: engine.show_dialogue("Today's the day of our big prank, remember?", callback = callback),
        lambda callback: engine.show_dialogue("Hehehehehehehehe...", callback = callback),
        lambda callback: heidi.wait(0.5, callback = callback),
        lambda callback: heidi.face_south(callback = callback),
        lambda callback: heidi.wait(0.5, callback = callback),
        lambda callback: engine.show_dialogue("Hahahahahahahahahahaha!", callback = callback),
        lambda callback: heidi.start_animating(callback = callback),
        lambda callback: engine.show_dialogue("MWAHAHAHAHAHAHAHAHAHAHAHAHA!!!", callback = callback),
        lambda callback: heidi.wait(0.5, callback = callback),
        lambda callback: heidi.stop_animating(callback = callback),
        lambda callback: heidi.wait(0.5, callback = callback),
        lambda callback: heidi.face_west(callback = callback),
        lambda callback: heidi.wait(0.5, callback = callback),
        lambda callback: engine.show_dialogue("Oops, sorry, got a bit carried away there. Anyway, let's head east to the merchant! He has all the stuff we need, then you can go to the bog to get the dirty bog water!", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback),
        lambda callback: heidi.follow(player_one, callback = callback)
    ]
elif level_data["heidi_state"] == heidi_state_getting_prank_materials:
    heidi.set_solidity(False)
    heidi.wait(0.3, callback = lambda: heidi.move_to(player_start_pos, callback = lambda: heidi.follow(player_one)))

""" end of everything too with heidi asking about the prank """

player_one.focus()

engine.play_music("calm")
engine.set_ui_colours((200,255,200),(215,255,215)) #TODO: save these colours in the config.

engine.disable_py_scripter()

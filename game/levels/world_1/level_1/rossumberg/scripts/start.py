#commence set-up
player_name = engine.get_player_name()
player_data = engine.get_player_data(player_name)
current_level = "/world_1/level_1/rossumberg"

#setting the player starting position before anything else based on which level the player was at previously
if player_data["current_level"] == "/world_1/level_1/player_house":
    player_one.move_to(exit_to_house.get_position(), callback = player_one.move_south)
elif player_data["current_level"] == "/world_1/level_1/road_one":
    player_one.move_to(exit_to_road_top.get_position(), callback = player_one.face_west)
else: #default level starting position
    player_one.move_to(exit_to_house.get_position(), callback = player_one.move_south)

player_data["current_level"] = current_level
# Add the basic level data to the save file if it doesn't exist
if not (current_level in player_data["level_states"]):
    player_data["level_states"][current_level] = {}

#A simple variable to use to refer to level state
level_state = player_data["level_states"][current_level]

if not ("heidi_has_spoken_about_prank" in level_state):
    level_state["heidi_has_spoken_about_prank"] = False
    
engine.save_player_data(player_name, player_data)

#end set-up

player_one.focus()

engine.play_music("calm")
engine.set_ui_colours((200,255,200),(215,255,215)) #TODO: save these colours in the config.

engine.disable_py_scripter()

""" set_up being able to change maps """
def go_to_house(player_object):
    engine.change_map("/world_1/level_1/player_house")

exit_to_house.player_walked_on = go_to_house

def go_to_road(player_object):
    engine.change_map("/world_1/level_1/road_one")

exit_to_road_top.player_walked_on = go_to_road
exit_to_road_bottom.player_walked_on = go_to_road

def heidi_player_action(player_object):
    player_one.set_busy(True)
    engine.show_dialogue("Remember, the merchant is East.", callback = lambda: player_one.set_busy(False))

heidi.player_action = heidi_player_action

route_sign.set_message("Rossumberg, where your mum is the village bicycle \n(east) merchant (east) bog")

""" LEVEL EVENTS """

""" Only have heidi speak to player about the prank if they haven't heard about it yet! """
if not level_state["heidi_has_spoken_about_prank"]:
    """ Heidi introducing herself and walking over to the player """
    heidi_introduction_sequence = [
        lambda callback: heidi.move_to(exit_to_road_top.get_position(), callback = callback),
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: player_one.face_south(callback = callback),
        lambda callback: heidi.face_west(callback = callback),
        lambda callback: engine.show_dialogue(engine.get_dialogue(current_level, "heidi_shout_player_name", {"player_name": player_name}), callback = callback),
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
            engine.get_dialogue(current_level, "heidi_today_is_big_day"),
            {
                engine.get_dialogue("shared", "yes"): lambda: engine.show_dialogue_with_options(
                    engine.get_dialogue(current_level, "heidi_question_player_about_day"),
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
        level_state["heidi_has_spoken_about_prank"] = True
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
else:
    heidi.follow(player_one)

""" end of everything too with heidi asking about the prank """

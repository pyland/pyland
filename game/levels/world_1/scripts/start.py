#TODO: remove a lot of the repetition from this code

#commence save-data set-up
world_name = "world_1"

update_world(world_name)

player_data.load(engine.get_player_name())
player_data.set_map(world_name) #change the map and save that the map has changed
#end save-data set-up

level_1_move_away = player_one.move_north
level_2_move_away = player_one.move_west
level_3_move_away = player_one.move_east
level_4_move_away = player_one.move_south
level_5_move_away = player_one.move_north
level_6_move_away = player_one.move_north
level_7_move_away = player_one.move_north
level_8_move_away = player_one.move_north


#setting the player's starting position
if player_data.previous_exit_is("world_1", level_name = "level_1"):
    if player_data.is_level_completed("/world_1/level_1"):
        player_one.move_to(level_1.get_position())
    else:
        player_one.move_to(level_1.get_position(), callback = level_1_move_away)
elif player_data.previous_exit_is("world_1", level_name = "level_2"):
    if player_data.is_level_completed("/world_1/level_2"):
        player_one.move_to(level_2.get_position())
    else:
        player_one.move_to(level_1.get_position(), callback = level_2_move_away)
elif player_data.previous_exit_is("world_1", level_name = "level_3"):
    if player_data.is_level_completed("/world_1/level_3"):
        player_one.move_to(level_3.get_position())
    else:
        player_one.move_to(level_3.get_position(), callback = level_3_move_away)
elif player_data.previous_exit_is("world_1", level_name = "level_4"):
    if player_data.is_level_completed("/world_1/level_4"):
        player_one.move_to(level_4.get_position())
    else:
        player_one.move_to(level_4.get_position(), callback = level_4_move_away)
elif player_data.previous_exit_is("world_1", level_name = "level_5"):
    if player_data.is_level_completed("/world_1/level_5"):
        player_one.move_to(level_5.get_position())
    else:
        player_one.move_to(level_5.get_position(), callback = level_5_move_away)
elif player_data.previous_exit_is("world_1", level_name = "level_6"):
    if player_data.is_level_completed("/world_1/level_6"):
        player_one.move_to(level_6.get_position())
    else:
        player_one.move_to(level_6.get_position(), callback = level_6_move_away)
elif player_data.previous_exit_is("world_1", level_name = "level_7"):
    if player_data.is_level_completed("/world_1/level_7"):
        player_one.move_to(level_7.get_position())
    else:
        player_one.move_to(level_7.get_position(), callback = level_7_move_away)
elif player_data.previous_exit_is("world_1", level_name = "level_8"):
    if player_data.is_level_completed("/world_1/level_8"):
        player_one.move_to(level_8.get_position())
    else:
        player_one.move_to(level_8.get_position(), callback = level_8_move_away)
#end setting the player's starting position

def ask_to_go_to_level(level_name, map_name, readable_name, move_method):
    player_one.set_busy(True)
    full_level_name = "/" + world_name + "/" + level_name
    map_location = full_level_name + "/" + map_name
    def player_refused_level():
        if not player_data.is_level_completed(full_level_name):
            player_one.set_busy(False, callback = move_method)
        else:
            player_one.set_busy(False)

    engine.show_dialogue_with_options(
        "Would you like go to " + readable_name + "?",
        {
            "Yes": lambda: player_data.save_and_exit(map_location),
            "No": player_refused_level
        }
    )

def go_to_level_1(player_object):
    ask_to_go_to_level("level_1", "player_house", "Level 1", level_1_move_away)

level_1.player_walked_on = go_to_level_1

def go_to_level_2(player_object):
    ask_to_go_to_level("level_2", "start", "Level 2", level_2_move_away)

level_2.player_walked_on = go_to_level_2

def go_to_level_3(player_object):
    ask_to_go_to_level("level_3", "main", "Level 3", level_3_move_away)

level_3.player_walked_on = go_to_level_3

def go_to_level_4(player_object):
    ask_to_go_to_level("level_4", "main_a", "Level 4", level_4_move_away)

level_4.player_walked_on = go_to_level_4

def go_to_level_5(player_object):
    ask_to_go_to_level("level_5", "maze", "Level 5", level_6_move_away)

level_5.player_walked_on = go_to_level_5

def go_to_level_6(player_object):
    ask_to_go_to_level("level_6", "vine_intro", "Level 6", level_7_move_away)

level_6.player_walked_on = go_to_level_6

def go_to_level_7(player_object):
    ask_to_go_to_level("level_7", "village", "Level 7", level_8_move_away)

level_7.player_walked_on = go_to_level_7

def go_to_level_8(player_object):
    pass
    #ask_to_go_to_level("/world_1/level_8/start", "Level 8")

level_8.player_walked_on = go_to_level_8

player_one.focus()
engine.play_music("world_1_jungle")

no_desert_guy.face_west()
no_desert_guy.set_dialogue_list(["The rest of Pyland is still under construction, so...", "...no desert for you."])

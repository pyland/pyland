#TODO: Add commands to PyGuide as you unlock ie face_north() etc

#The red collisions tiles have been removed from the map for testing purposes

#commence save-data set-up
world_name = "world1"

engine.update_world_text("1")
engine.clear_level_text()

player_one.focus()
engine.play_music("world_1_jungle")

player_data.load(engine.get_player_name())
player_data.set_map(world_name) #change the map and save that the map has changed
#end save-data set-up

level_1_move_away = player_one.move_west
level_2_move_away = player_one.move_west
level_3_move_away = player_one.move_east
level_4_move_away = player_one.move_south
level_5_move_away = player_one.move_west
level_6_move_away = player_one.move_north
level_7_move_away = player_one.move_east
level_8_move_away = player_one.move_west
level_8_move_away = player_one.move_west

level_1_2_move_away = player_one.move_west
level_2_2_move_away = player_one.move_west

level_demo_move_away = player_one.move_west


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
elif player_data.previous_exit_is("world_1", level_name = "level_9"):
    if player_data.is_level_completed("/world_1/level_9"):
        player_one.move_to(level_9.get_position())
    else:
        player_one.move_to(level_9.get_position(), callback = level_9_move_away)
elif player_data.previous_exit_is("world_2", level_name = "level_1"):
    if player_data.is_level_completed("/world_2/level_1"):
        player_one.move_to(level_1_2.get_position())
    else:
        player_one.move_to(level_1_2.get_position(), callback = level_1_2_move_away)
elif player_data.previous_exit_is("world_2", level_name = "level_2"):
    if player_data.is_level_completed("/world_2/level_2"):
        player_one.move_to(level_2_2.get_position())
    else:
        player_one.move_to(level_2_2.get_position(), callback = level_2_2_move_away)
elif player_data.previous_exit_is("demo_levels", level_name = "level_1"):
    if player_data.is_level_completed("/demo_levels/level_1"):
        player_one.move_to(level_1_2.get_position())
    else:
        player_one.move_to(level_1_2.get_position(), callback = level_1_2_move_away)


#end setting the player's starting position

def ask_to_go_to_level(world_name,level_name, map_name, readable_name, move_method):
    player_one.set_busy(True)
    full_level_name = "/" + world_name + "/" + level_name
    map_location = full_level_name + "/" + map_name
    def player_refused_level():
        #REMOVED FOR TESTING
        #if not player_data.is_level_completed(full_level_name):
        #    player_one.set_busy(False, callback = move_method)
        #else:
            player_one.set_busy(False)

    engine.show_dialogue_with_options(
        "Would you like go to " + readable_name + "?",
        {
            "Yes": lambda: player_data.save_and_exit(map_location),
            "No": player_refused_level
        }
    )

def go_to_level_1(player_object):
    ask_to_go_to_level("world1","level1", "main", "Level 1", level_1_move_away)

level_1.player_walked_on = go_to_level_1

def go_to_level_2(player_object):
    ask_to_go_to_level("world1","level2", "main_a", "Level 2", level_2_move_away)

level_2.player_walked_on = go_to_level_2

def go_to_level_3(player_object):
    ask_to_go_to_level("world1","level3", "main", "Level 3", level_3_move_away)

level_3.player_walked_on = go_to_level_3

def go_to_level_4(player_object):
    ask_to_go_to_level("world1","level4", "main", "Level 4", level_4_move_away)

level_4.player_walked_on = go_to_level_4

def go_to_level_5(player_object):
    ask_to_go_to_level("world1","level5", "main", "Level 5", level_6_move_away)

level_5.player_walked_on = go_to_level_5

def go_to_level_6(player_object):
    ask_to_go_to_level("world1","level6", "main", "Level 6", level_6_move_away)

level_6.player_walked_on = go_to_level_6

def go_to_level_7(player_object):
    ask_to_go_to_level("world1","level7", "maze", "Level 7", level_7_move_away)

level_7.player_walked_on = go_to_level_7

def go_to_level_8(player_object):
    ask_to_go_to_level("world1","level8", "vine_intro", "Level 8", level_8_move_away)

level_8.player_walked_on = go_to_level_8

def go_to_level_9(player_object):
    ask_to_go_to_level("world1","level9", "main", "Level9", level_9_move_away)

level_9.player_walked_on = go_to_level_9

def go_to_level_1_2(player_object):
    ask_to_go_to_level("world2","level1", "main", "Level 1", level_1_2_move_away)

level_1_2.player_walked_on = go_to_level_1_2

#Loading Ying's World 2 Level 4 for now

def go_to_level_2_2(player_object):
    ask_to_go_to_level("world2","level4", "main", "Level 2", level_2_2_move_away)

level_2_2.player_walked_on = go_to_level_2_2


def go_to_level_demo(player_object):
    ask_to_go_to_level("demo_levels","level_1", "main", "Demo Level", level_demo_move_away)

level_demo.player_walked_on = go_to_level_demo

def change_world(world):
    engine.update_world_text(str(world))
    if world ==  1:
        engine.play_music("world_1_jungle")
    elif world ==  2:
        engine.play_music("eery") #will be desert theme

world_1.player_walked_on = lambda player_one: change_world(1)
world_2.player_walked_on = lambda player_one: change_world(2)

no_desert_guy.face_west()
no_desert_guy.set_dialogue_list(["That's it for the Pyland levels so far. You can try this demo level which will be taught later in the game!"])

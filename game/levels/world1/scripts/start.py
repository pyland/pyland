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

level1_move_away = player_one.move_west
level2_move_away = player_one.move_south
level3_move_away = player_one.move_east
level4_move_away = player_one.move_south
level5_move_away = player_one.move_west
level6_move_away = player_one.move_north

level1_2_move_away = player_one.move_west
level3_2_move_away = player_one.move_west

level_demo_move_away = player_one.move_west


#setting the player's starting position
if player_data.previous_exit_is("world1", level_name = "level1"):
    if player_data.is_level_completed("/world1/level1"):
        player_one.move_to(level1.get_position())
    else:
        player_one.move_to(level1.get_position(), callback = level1_move_away)
elif player_data.previous_exit_is("world1", level_name = "level2"):
    if player_data.is_level_completed("/world1/level2"):
        player_one.move_to(level2.get_position())
    else:
        player_one.move_to(level2.get_position(), callback = level2_move_away)
elif player_data.previous_exit_is("world1", level_name = "level3"):
    if player_data.is_level_completed("/world1/level3"):
        player_one.move_to(level3.get_position())
    else:
        player_one.move_to(level3.get_position(), callback = level3_move_away)
elif player_data.previous_exit_is("world1", level_name = "level4"):
    if player_data.is_level_completed("/world1/level4"):
        player_one.move_to(level4.get_position())
    else:
        player_one.move_to(level4.get_position(), callback = level4_move_away)
elif player_data.previous_exit_is("world1", level_name = "level5"):
    if player_data.is_level_completed("/world1/level5"):
        player_one.move_to(level5.get_position())
    else:
        player_one.move_to(level5.get_position(), callback = level5_move_away)
elif player_data.previous_exit_is("world1", level_name = "level6"):
    if player_data.is_level_completed("/world1/level6"):
        player_one.move_to(level6.get_position())
    else:
        player_one.move_to(level6.get_position(), callback = level6_move_away)
elif player_data.previous_exit_is("world2", level_name = "level1"):
    if player_data.is_level_completed("/world2/level1"):
        player_one.move_to(level1_2.get_position())
    else:
        player_one.move_to(level1_2.get_position(), callback = level1_2_move_away)
elif player_data.previous_exit_is("world2", level_name = "level3"):
    if player_data.is_level_completed("/world2/level3"):
        player_one.move_to(level3_2.get_position())
    else:
        player_one.move_to(level2_2.get_position(), callback = level2_2_move_away)
elif player_data.previous_exit_is("demo_levels", level_name = "level1"):
    if player_data.is_level_completed("/demo_levels/level1"):
        player_one.move_to(level1_2.get_position())
    else:
        player_one.move_to(level1_2.get_position(), callback = level1_2_move_away)


#end setting the player's starting position

def ask_to_go_to_level(world_name,level_name, map_name, readable_name, move_method):
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

def go_to_level1(player_object):
    ask_to_go_to_level("world1","level1", "player_house", "Level 1", level1_move_away)

level1.player_walked_on = go_to_level1

def go_to_level2(player_object):
    ask_to_go_to_level("world1","level2", "start", "Level 2", level2_move_away)

level2.player_walked_on = go_to_level2

def go_to_level3(player_object):
    ask_to_go_to_level("world1","level3", "main", "Level 3", level3_move_away)

level3.player_walked_on = go_to_level3

def go_to_level4(player_object):
    ask_to_go_to_level("world1","level4", "main", "Level 4", level4_move_away)

level4.player_walked_on = go_to_level4

def go_to_level5(player_object):
    ask_to_go_to_level("world1","level5", "main", "Level 5", level5_move_away)

level5.player_walked_on = go_to_level5

def go_to_level6(player_object):
    ask_to_go_to_level("world1","level6", "main", "Level 6", level6_move_away)

level6.player_walked_on = go_to_level6

def go_to_level1_2(player_object):
    ask_to_go_to_level("world2","level1", "main", "Level 1", level1_2_move_away)

level1_2.player_walked_on = go_to_level1_2

def go_to_level3_2(player_object):
    ask_to_go_to_level("world2","level3", "main", "Level 3", level3_2_move_away)

level3_2.player_walked_on = go_to_level3_2


def go_to_level_demo(player_object):
    ask_to_go_to_level("demo_levels","level1", "main", "Demo Level", level_demo_move_away)

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

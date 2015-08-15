""" Load in saved states """

player_one.focus()
player_one.set_character_name("Benji")
player_two.set_character_name("Bob")

c1 = (255, 255, 255)
c2 = (245, 245, 245)
engine.set_ui_colours(c1, c2)
#engine.print_terminal("Switching level")
#engine.change_map("test_world/yingischallenged/main_2")

print(engine.print_terminal(engine.get_script()))

engine.clear_scripter()
engine.insert_to_scripter("howdy")
engine.insert_to_scripter("partner")
engine.insert_to_scripter("print(<Your name here>)")

engine.update_world("Jungle")
engine.update_level("Third")
engine.update_coins(42)
engine.update_totems(3,True)

print(engine.print_terminal(engine.get_script()))

coconut_one.set_weight(1)
coconut_two.set_weight(2)
coconut_three.set_weight(4)
coconut_four.set_weight(8)
coconut_five.set_weight(16)
coconut_six.set_weight(32)
coconut_seven.set_weight(64)

coconut_one.set_solidity(False);

engine.print_terminal(coconut_one.get_weight(), False)

#player_9000 = engine.create_object("characters/enemies/crocodile", "player_9000", (10, 10))

#player_9000 = engine.create_object("characters/player", "player_9000", (10, 10))
#player_9000.move_south(None)
#engine.print_debug("bobobobobobo")

engine.play_music("beach")
engine.get_objects_at((7, 2))
engine.get_objects_at((4, 4))
engine.get_objects_at((5, 4))
engine.get_objects_at((6, 4))
engine.get_objects_at((1, 1))
engine.get_objects_at((7, 4))
engine.get_objects_at((8, 4))
engine.get_objects_at((9, 4))
engine.get_objects_at(player_one.get_position())

#engine.change_map(map_name)
#call finish.py

engine.add_dialogue(engine.get_dialogue("welcome"))

#engine.open_dialogue_box(coconut_one.focus)

croc_one.follow_path("north, east, east, north, east, south, south, south, west, west, west, north", True)
croc_two.rand_explore()
croc_three.rand_explore()
croc_four.rand_explore()

#engine.print_terminal(player_one.get_position(), False)

#engine.print_terminal(engine.getDialogue("welcome"))

#engine.print_terminal(engine.get_tile_type((2,16)))
#engine.print_terminal(engine.get_tile_type((3,16)))
#engine.print_terminal(engine.get_tile_type((4,16)))
#engine.print_terminal(engine.get_tile_type((5,16)))


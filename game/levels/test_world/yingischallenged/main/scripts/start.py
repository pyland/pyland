""" Load in saved states """

player_one.focus()

coconut_one.set_weight(1)
coconut_two.set_weight(2)
coconut_three.set_weight(4)
coconut_four.set_weight(8)
coconut_five.set_weight(16)
coconut_six.set_weight(32)
coconut_seven.set_weight(64)

engine.print_terminal(str(coconut_one.get_weight()) + "\n", False)

engine.play_music("beach")
engine.get_objects_at((7, 0))
engine.get_objects_at((7, 2))
engine.get_objects_at((4, 4))
engine.get_objects_at((5, 4))
engine.get_objects_at((6, 4))
engine.get_objects_at((1, 1))
engine.get_objects_at((7, 4))
engine.get_objects_at((8, 4))
engine.get_objects_at((9, 4))
engine.get_objects_at(player_one.get_position())

engine.add_button("gui/head/monkey", "Ben", player_one.focus)
engine.add_button("gui/head/monkey", "Rock1", coconut_one.focus)
engine.add_button("gui/head/monkey", "Rock2", coconut_two.focus)
engine.add_button("gui/head/monkey", "Rock3", coconut_three.focus)
engine.add_button("gui/head/monkey", "Rock4", coconut_four.focus)
engine.add_button("gui/head/monkey", "Rock5", coconut_five.focus)
engine.add_button("gui/head/monkey", "Rock6", coconut_six.focus)
engine.add_button("gui/head/monkey", "Rock7", coconut_seven.focus)
engine.add_button("gui/head/monkey", "Portal", exit_portal.focus)

engine.print_terminal(str(player_one.get_position()) + "\n", False)

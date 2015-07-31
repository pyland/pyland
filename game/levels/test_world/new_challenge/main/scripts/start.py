import threading
#import Player from characters/player

#player_one = Player()
""" Load in saved states """

#print("Creating the level")
#player_one.move_north()

#croc_one.follow_path("north, east, south")

#portal_one.set_destination("level_two")


#boulder_one.move_north()

#boulder_one.callback_test(lambda: boulder_one.callback_test(lambda: print("hohohoho")))


#player_one.focus()
#player_one.run_script("1")

boulder_one.focus()
boulder_one.move_south(lambda: boulder_one.move_south(lambda: boulder_one.move_west(lambda: boulder_one.move_south(lambda: boulder_one.move_south(lambda: boulder_one.move_south())))))
#croc_one.focus()
#croc_one.follow_path("north, east, east, south, west, west, west, west, north, south", True)
#croc_one.rand_explore()


#boulder_four.callback_test(lambda: boulder_four.callback_test(lambda: boulder_four.callback_test(lambda: print(dialogue))))
boulder_four.callback_test(lambda: boulder_four.callback_test(lambda: boulder_four.callback_test(lambda: print(game.getDialogue("welcome")))))
#print("wohooo")

engine.add_button("gui/coin/coin-tile", "Ben", player_one.focus)
engine.add_button("gui/coin/coin-tile", "Rock1", boulder_one.focus)
engine.add_button("gui/coin/coin-tile", "Rock2", boulder_two.focus)
engine.add_button("gui/coin/coin-tile", "Rock3", boulder_three.focus)
engine.add_button("gui/coin/coin-tile", "Rock4", boulder_four.focus)
engine.add_button("gui/coin/coin-tile", "Rock5", boulder_five.focus)
engine.add_button("gui/coin/coin-tile", "Portal", portal_one.focus)

engine.play_music()


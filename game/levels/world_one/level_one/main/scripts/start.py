player_one.focus()

engine.play_music("beach")


croc_one.rand_explore()
croc_two.rand_explore()
croc_three.rand_explore()
croc_four.rand_explore()
croc_five.rand_explore()


dialogue_sequence = [
    lambda callback: engine.show_dialogue("Welcome to level one!", callback),
    lambda callback: engine.show_dialogue("Use your WASD keys to move to the end of the path!", callback),
    #TODO: change the level to the intro level
]


end_dialogue = [
    lambda callback: engine.show_dialogue("Congratulations! You've reached the end!", callback),
    lambda callback: engine.show_dialogue("Here is a funny joke! \n What do you call an alligator in a vest?", callback),
    #TODO: change the level to the intro level
    lambda callback: engine.show_dialogue("An investigator!", callback),
]

end.dialogue = end_dialogue
engine.run_callback_list_sequence(dialogue_sequence)



player_one.focus()

engine.play_music("beach")




dialogue_sequence = [
    lambda callback: engine.show_dialogue("Welcome to level two!", callback),
    lambda callback: engine.show_dialogue("Walk to the first checkpoint over there!", callback),
    #TODO: change the level to the intro level
]


dialogue_checkpoint_one = [
    lambda callback: engine.show_dialogue("Great! But we can also walk by writing code.", callback),
    lambda callback: engine.show_dialogue("Type move_east() in the codeblock and press run to see what happens!", callback),
    #TODO: change the level to the intro level
    lambda callback: engine.show_dialogue("I'll meet you at the next checkpoint!", callback),
]

dialogue_checkpoint_two = [
    lambda callback: engine.show_dialogue("Try typing move_east() into the codeblock many times. Make sure they are on separate lines!", callback),
    lambda callback: engine.show_dialogue("Toggle the speed option to fast and run the code!", callback),
    #TODO: change the level to the intro level
    lambda callback: engine.show_dialogue("I'll meet you at the end!", callback),
]

dialogue_checkpoint_end = [
    lambda callback: engine.show_dialogue("You've reached the end!", callback),
    lambda callback: engine.show_dialogue("We can also move in other directions by typing in move_north(), move_west(), and move_south() into the codeblock.", callback),
    #TODO: change the level to the intro level
    lambda callback: engine.show_dialogue("The run button runs all the current code in the codeblock and you can change it to do whatever you want!", callback),
    lambda callback: engine.show_dialogue("What do you call a snake that likes to program?", callback),
    lambda callback: engine.show_dialogue("A code-bra!", callback),
]


cp_one.dialogue = dialogue_checkpoint_one
cp_two.dialogue = dialogue_checkpoint_two
cp_end.dialogue = dialogue_checkpoint_end
engine.run_callback_list_sequence(dialogue_sequence)



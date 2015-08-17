player_one.focus()

engine.play_music("beach")


engine.clear_scripter()

player_one.face_east()

dialogue_sequence = [
    lambda callback: player_one.set_busy(True, callback = callback),
    lambda callback: engine.show_dialogue("Welcome to level two!", callback = callback),
    lambda callback: engine.show_dialogue("Walk to the first checkpoint over there!", callback = callback),
    lambda callback: player_one.set_busy(False, callback = callback)
]


dialogue_checkpoint_one = [
    lambda callback: player_one.set_busy(True, callback = callback),
    lambda callback: engine.show_dialogue("Great! But we can also walk by writing code.", callback = callback),
    lambda callback: engine.show_dialogue("A script will appear in the PySripter, press 'run' to see what happens once it has!", callback = callback),
    lambda callback: engine.show_dialogue("I'll meet you at the next checkpoint!", callback = callback),
    lambda callback: player_one.set_busy(False, callback = callback),
    lambda callback: engine.clear_scripter(callback = callback),
    lambda callback: engine.insert_to_scripter("move_east()", callback = callback)
]

dialogue_checkpoint_two = [
    lambda callback: player_one.set_busy(True, callback = callback),
    lambda callback: engine.show_dialogue("Try typing move_east() into the codeblock many times. Make sure they are on separate lines! So you can run a big block at once!", callback = callback),
    lambda callback: engine.show_dialogue("If you press on the \"speed\" button in the very bottom right-hand corner of the screen, you will be able to run your code super-fast.", callback = callback),
    lambda callback: engine.show_dialogue("I'll meet you at the end!", callback = callback),
    lambda callback: player_one.set_busy(False, callback = callback)
]

dialogue_checkpoint_end = [
    lambda callback: player_one.set_busy(True, callback = callback),
    lambda callback: engine.show_dialogue("You've reached the end!", callback = callback),
    lambda callback: engine.show_dialogue("We can also move in other directions by typing in move_north(), move_west(), and move_south() into the codeblock.", callback = callback),
    lambda callback: engine.show_dialogue("The run button runs all the current code in the codeblock and you can change it to do whatever you want!", callback = callback),
    lambda callback: engine.show_dialogue("What do you call a snake that likes to program?", callback = callback),
    lambda callback: engine.show_dialogue("A code-bra!", callback),
    lambda callback: player_one.set_busy(False, callback = callback)

]


cp_one.dialogue = dialogue_checkpoint_one
cp_two.dialogue = dialogue_checkpoint_two
cp_end.dialogue = dialogue_checkpoint_end
engine.run_callback_list_sequence(dialogue_sequence)



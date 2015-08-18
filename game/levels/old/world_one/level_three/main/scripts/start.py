player_one.focus()

engine.play_music("beach")
player_one.face_east()



dialogue_start = [
    lambda callback: player_one.set_busy(True, callback = callback),
    lambda callback: engine.show_dialogue("Typing move_east() multiple times does get a bit boring doesn't it?", callback = callback),
    lambda callback: engine.show_dialogue("Let's make simpler by using a concept called a loop!", callback = callback),
    lambda callback: engine.clear_scripter(callback = callback),
    lambda callback: engine.insert_to_scripter("for i in range(20):\n  move_east()", callback = callback),
    lambda callback: engine.show_dialogue("The code below will be equivalent to if you had typed move_east() 20 times!", callback = callback),
    lambda callback: engine.show_dialogue("Run it to see what happens!", callback = callback),
    lambda callback: player_one.set_busy(False, callback = callback),
    
]

engine.run_callback_list_sequence(dialogue_start)


checkpoint_one.dialogue = [
    lambda callback: player_one.set_busy(True, callback = callback),
    lambda callback: engine.show_dialogue("Oh no! An intersection! Which way do we go?", callback = callback),
    lambda callback: engine.show_dialogue("Worry not! Those blue flags are called message flags, and they will assist you!", callback = callback),
    lambda callback: engine.show_dialogue("I've put some code in the PyScripter that will let you read the flag's message.", callback = callback),
    lambda callback: engine.clear_scripter(callback = callback),
    lambda callback: engine.insert_to_scripter("print(get_flag_message())", callback = callback),
    lambda callback: engine.show_dialogue("Walk on top of the blue flag and run the PyScripter", callback = callback),
    lambda callback: engine.show_dialogue("Don't forget! You can use for loops to easily walk long distances!", callback = callback),
    lambda callback: player_one.set_busy(False, callback = callback),
]

message_one.set_message("The beast goes east!")
message_two.set_message("No doubt! Go south!")
message_two.set_message("It's best to go west!")
message_four.set_message("It's best to go west!")
message_five.set_message("The right course is north!")
message_six.set_message("It's best to go west!")
message_seven.set_message("No doubt! Go south!")
message_eight.set_message("The beast goes east!")

def gary_action(player_object):
    if(player_object.is_facing_south()):
        gary.face_north()
    elif(player_object.is_facing_east()):
        gary.face_west()
    elif(player_object.is_facing_north()):
        gary.face_south()
    else:
        gary.face_east()
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("I was just playing with my friend Ash,", callback = callback),
        lambda callback: engine.show_dialogue("I wonder where he's gone", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback),
    ])

gary.player_action = gary_action

def ash_action(player_object):
    if(player_object.is_facing_south()):
        ash.face_north()
    elif(player_object.is_facing_east()):
        ash.face_west()
    elif(player_object.is_facing_north()):
        ash.face_south()
    else:
        ash.face_east()
    engine.run_callback_list_sequence([
        lambda callback: engine.show_dialogue("I'M REALLY LOST AND SCARED", callback = callback),
    ])

ash.player_action = ash_action
gary.face_west()
ash.face_east()

end.dialogue = [
    lambda callback: engine.show_dialogue("We've made it!", callback = callback),
    lambda callback: engine.show_dialogue("It's nifty that print statements can be used to display information", callback = callback),
    lambda callback: engine.show_dialogue("and that for loops can be used to make code short and consise.", callback = callback),
]



#end.dialogue = end_dialogue


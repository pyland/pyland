from random import randint

player.focus()

engine.play_music("eery")
engine.update_level("7");
#engine.disable_py_scripter()

#engine.clear_scripter()


#engine.set_py_tabs(6)
#engine.set_ui_colours((0,0,0),(10,10,10))

player.face_east()
player.set_busy(False)
myla.face_east()

leader.face_west()

snake1.face_east()
snake2.face_east()
snake3.face_east()
snake4.face_east()
snake5.face_east()
snake6.face_east()
snake7.face_east()

snake1.nervous_flag = True
snake2.nervous_flag = True
snake3.nervous_flag = True
snake4.nervous_flag = True
snake5.nervous_flag = True
snake6.nervous_flag = True
snake7.nervous_flag = True

def nervous(cur_object, time = 0.5, callback = lambda: None):
    direction = randint(0,8)
    if (not cur_object.nervous_flag):
        pass
    elif direction == 0:
        cur_object.face_north(lambda: cur_object.wait(time, callback = lambda: nervous(cur_object, time)))
    elif direction == 1:
        cur_object.face_east(lambda: cur_object.wait(time, callback = lambda: nervous(cur_object, time)))
    elif direction == 2:
        cur_object.face_south(lambda: cur_object.wait(time, callback = lambda: nervous(cur_object, time)))
    elif direction == 3:
        cur_object.face_west(lambda: cur_object.wait(time, callback = lambda: nervous(cur_object, time)))
    else:
        cur_object.wait(time, callback = lambda: nervous(cur_object, time))
    callback()

def pace(cur_object, callback = lambda: None):
    cur_object.move_north(lambda: cur_object.wait(1.0, lambda: cur_object.move_south(lambda: cur_object.move_south(lambda: cur_object.wait(1.0, lambda: cur_object.move_north(lambda: pace(cur_object)))))))
    callback()

#nervous()

#snake1.face_north(lambda: snake1.wait(2.0, callback = snake1.face_south))

player.set_busy(True)

dialogue_sequence = [
    lambda callback: engine.show_dialogue("Minister: Attention my friends. I am sorry to have to say this again.", callback = callback),
    lambda callback: engine.show_dialogue("The catchers are back.", callback = callback),
    lambda callback: nervous(snake1, time = 0.1, callback = callback),
    lambda callback: nervous(snake2, time = 0.1, callback = callback),
    lambda callback: nervous(snake3, time = 0.1, callback = callback),
    lambda callback: nervous(snake4, time = 0.1, callback = callback),
    lambda callback: nervous(snake5, time = 0.1, callback = callback),
    lambda callback: nervous(snake6, time = 0.1, callback = callback),
    lambda callback: nervous(snake7, time = 0.1, callback = callback),
    lambda callback: leader.move_north(callback = callback),
    lambda callback: leader.move_west(callback = callback),
    lambda callback: leader.move_west(callback = callback),
    lambda callback: leader.move_south(callback = callback),
    lambda callback: leader.face_west(callback = callback),
    lambda callback: engine.show_dialogue("Please stay calm. I can handle this.", callback = callback),
    lambda callback: engine.show_dialogue("But we must prepare to escape to the desert, in case they do not leave.", callback = callback),
    lambda callback: myla.move_east(callback = callback),
    lambda callback: engine.show_dialogue("Myla: Lets help them out, talk to as many of them as you can.", callback = callback),
    lambda callback: pace(leader, callback = callback),

    lambda callback: player.set_busy(False, callback = callback),
   # lambda callback: engine.enable_py_scripter(callback = callback)
]

player1_sequence = [
    lambda callback: engine.show_dialogue("I'm scared. I thought the catchers were just a myth.", callback = callback),
    lambda callback: engine.show_dialogue("My PyRunner script is broken. Can you fix it so I move east and get to the desert?", callback = callback),
    #Ask to help
    #Have variable to determine number have helped
    #Myla block the door if you don't
    #Add callbacks to all
    lambda callback: leader.move_north(callback = callback),
    lambda callback: leader.move_west(callback = callback),
    lambda callback: leader.move_west(callback = callback),
    lambda callback: leader.move_south(callback = callback),
    lambda callback: leader.face_west(callback = callback),
    lambda callback: engine.show_dialogue("Please stay calm. I can handle this.", callback = callback),
    lambda callback: engine.show_dialogue("But we must prepare to escape to the desert, in case they do not leave.", callback = callback),
    lambda callback: myla.move_east(callback = callback),
    lambda callback: engine.show_dialogue("Myla: Lets help them out, talk to as many of them as you can.", callback = callback),
    lambda callback: pace(leader, callback = callback),

    lambda callback: player.set_busy(False, callback = callback),
    lambda callback: engine.enable_py_scripter(callback = callback)
]

def snake1_player_action(player_object):
    xP, yP = player_object.get_position()
    xS, yS = snake1.get_position()
    xDiff = xP - xS
    yDiff  = yP - yS

    if xDiff == 1:
        snake1.face_east()
    elif xDiff == - 1:
        snake1.face_west()
    elif yDiff == 1:
        snake1.face_north()
    elif yDiff == -1:
        snake1.face_south()
    snake1.nervous_flag = False
    engine.run_callback_list_sequence(player1_sequence)

snake1.player_action = snake1_player_action

engine.run_callback_list_sequence(dialogue_sequence)

engine.enable_py_scripter()

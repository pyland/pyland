#commence save-data set-up
world_name = "world1"
level_name = "level5"
map_name = "main"

engine.update_world_text("1")
engine.update_level_text("2")

player_data.load(engine.get_player_name())
player_data.set_map(world_name, level_name = level_name, map_name = map_name) #change the map and save that the map has changed
#end save-data set-up

#defining verious functions

def myla_explain_cut_action(player_object):
    player_one.set_cuts_left(2)
    myla_explain_cut.player_walked_on = lambda player_object: None
    player_data.save_checkpoint("myla_explain_cut")

    def myla_ask_player_if_there_is_way(callback):
        engine.show_dialogue_with_options(
            "...",
            {
                "Is there a way?" : lambda: engine.show_dialogue("Why thank you, you can use your *ahem*, I mean MY PyScripter!", callback = callback),
                "...": lambda: engine.show_dialogue("Argh ok you win. You can use the PyScripter.", callback = callback)
            }
        )

    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: player_one.face_east(callback = callback),
        lambda callback: engine.show_dialogue(engine.get_player_name() + ", there's a vine in the way over there.", callback = callback),
        lambda callback: engine.show_dialogue("What's that, I hear you ask? How could we possibly make it through?", callback = callback),
        lambda callback: engine.show_dialogue("If only there was a way...", callback = callback),
        myla_ask_player_if_there_is_way,
        lambda callback: engine.show_dialogue("I'm going to give you my knife so that you can cut down vines.", callback = callback),
        lambda callback: engine.show_dialogue("The PyScripter let's you control objects from the real world. So you can use it to cut down vines!", callback = callback),
        lambda callback: engine.show_dialogue("I have written a small program for you and put it into your PyScripter...", callback = callback),
        lambda callback: engine.show_dialogue("Stand next to the vine and run your script to cut it down!", callback = callback),
        lambda callback: engine.clear_scripter(callback = callback),
        lambda callback: engine.insert_to_scripter("cut()\n", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback)
    ])


def myla_explain_whetstone_action(player_object):
    myla_explain_whetstone.player_walked_on = lambda player_object: None
    player_data.save_checkpoint("myla_explain_whetstone")
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: myla.stop_follow(callback = callback),
        lambda callback: myla.set_solidity(True, callback = callback),
        lambda callback: myla.wait(0.3, callback = callback),
        lambda callback: myla.move_east(callback = callback),
        lambda callback: myla.face_north(callback = callback),
        lambda callback: player_one.face_south(callback = callback),
        lambda callback: engine.show_dialogue("Well done " + engine.get_player_name() + "!", callback = callback),
        lambda callback: engine.show_dialogue("You might have noticed that in the PyConsole the number of 'cuts' you have left decreased when you cut the vines.", callback = callback),
        lambda callback: engine.show_dialogue("That's because the knife gets more blunt everytime you use it!", callback = callback),
        lambda callback: myla.face_east(callback = callback),
        lambda callback: player_one.face_east(callback = callback),
        lambda callback: engine.show_dialogue("You see that object over there? That's a whetstone.", callback = callback),
        lambda callback: myla.face_north(callback = callback),
        lambda callback: player_one.face_south(callback = callback),
        lambda callback: engine.show_dialogue("If you interact with it, it will replenish the amount of vines you will be able to cut!", callback = callback),
        lambda callback: engine.show_dialogue("The quality of the whetstone determines how much your knife will be sharpened.", callback = callback),
        lambda callback: engine.show_dialogue("Also, all the vines in the area will grow back when you sharpen your knife.", callback = callback),
        lambda callback: myla.follow(player_one, callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback)
    ])


myla_in_bag = True
def myla_go_in_bag(player_object):
    #TODO: make it so that myla actually pauses is the player and gets in/out of their bag
    global myla_in_bag
    if (myla_in_bag):
        myla.set_visible(True)
        myla_in_bag = False
    else:
        myla.set_visible(False)
        myla_in_bag = True

def myla_explain_crocodiles_action(player_object):
    myla_explain_crocodiles.player_walked_on = myla_go_in_bag
    player_data.save_checkpoint("myla_explain_crocodiles")
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: player_one.wait(0.3, callback = callback),
        lambda callback: myla.stop_follow(callback = callback),
        lambda callback: myla.set_solidity(True, callback = callback),
        lambda callback: myla.move_south(callback = callback),
        lambda callback: myla.move_south(callback = callback),
        lambda callback: myla.set_solidity(False, callback = callback),
        lambda callback: player_one.face_east(callback = callback),
        lambda callback: myla.face_west(callback = callback),
        lambda callback: engine.show_dialogue("Swinging monkeys! Seems like the Carl's lads and lasses are already out.", callback = callback),
        lambda callback: engine.show_dialogue("They seem to be sleeping. But if you walk next to them I'm sure they will wake up, so cut down the vines to avoid them!", callback = callback),
        lambda callback: engine.show_dialogue("Also.. I might be a bit slightly too clumsly and loud...", callback = callback),
        lambda callback: engine.show_dialogue("So I'm just gonna...", callback = callback),
        lambda callback: engine.show_dialogue("Ummm... you know...", callback = callback),
        lambda callback: engine.show_dialogue("Just hop into your bag if that's ok...", callback = callback),
        lambda callback: engine.show_dialogue("Thank you!", callback = callback),
        lambda callback: player_one.face_west(callback = callback),
        lambda callback: myla.move_west(callback = callback),
        lambda callback: myla.set_visible(False, callback = callback),
        lambda callback: myla.wait(0.3, callback = callback),
        lambda callback: engine.show_dialogue("Oh nice, you have a banana in here.", callback = callback),
        lambda callback: engine.show_dialogue("Arrgh old banana, old moldy banana! Eurgh, eurgh!", callback = callback),
        lambda callback: engine.show_dialogue("You really need to clear your bag out sometime " + engine.get_player_name() + "!", callback = callback),
        lambda callback: myla.follow(player_one, callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback)
    ])

def myla_explain_basic_move_action(player_object):
    myla_explain_basic_move.player_walked_on = myla_go_in_bag
    player_data.save_checkpoint("myla_explain_basic_move")
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: myla.stop_follow(callback = callback),
        lambda callback: myla.wait(0.3, callback = callback),
        lambda callback: myla.set_solidity(False, callback = callback),
        lambda callback: myla.move_to(player_one.get_position(), callback = callback),
        lambda callback: myla.set_visible(True, callback = callback),
        lambda callback: myla.move_west(callback = callback),
        lambda callback: myla.face_east(callback = callback),
        lambda callback: player_one.face_west(callback = callback),
        lambda callback: engine.show_dialogue("So far you have learnt how two functions " + engine.get_player_name() + ", cut and print.", callback = callback),
        lambda callback: engine.show_dialogue("As you learn more and more, you probably won't remember all of them or how to use them.", callback = callback),
        lambda callback: engine.show_dialogue("This is where the PyGuide comes in handy!", callback = callback),
        lambda callback: engine.show_dialogue("Click on your bag in the top-right hand corner to open it. Click on it again to close it. You won't be able to open it until I finish talking to you though.", callback = callback),
        lambda callback: engine.show_dialogue("When you open it you will see something called the PyGuide, click on that to open it.", callback = callback),
        lambda callback: engine.show_dialogue("You will find that it has all the functions that you have learned so far in it, along with a description of what it does and how to use it.", callback = callback),
        lambda callback: engine.show_dialogue("There will also be four that you've never used before. Give them a try and see what they do!", callback = callback),
        lambda callback: engine.show_dialogue("As you learn more about the PyScripter, you will learn more and more functions. So getting familiar with the PyGuide is very usefull!", callback = callback),
        lambda callback: myla.follow(player_one, callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback)
    ])

def myla_give_simple_loop_action(player_object):
    myla_give_simple_loop.player_walked_on = lambda player_object: None
    player_data.save_checkpoint("myla_give_simple_loop")
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: myla.set_solidity(True, callback = callback),
        lambda callback: myla.stop_follow(callback = callback),
        lambda callback: myla.move_south(callback = callback),
        lambda callback: myla.move_east(callback = callback),
        lambda callback: player_one.face_west(callback = callback),
        lambda callback: engine.show_dialogue("I'm going to teach you a new Python concept now called a loop.", callback = callback),
        lambda callback: engine.show_dialogue("A loop is the easiest way of telling a program to do the same things multiple times.", callback = callback),
        lambda callback: engine.show_dialogue("We will also be using a new set of functions that let you move using the PyScripter!", callback = callback),
        lambda callback: engine.show_dialogue("If you were keen and looked in the PyGuide you might have spotted them there! They let you move about.", callback = callback),
        lambda callback: engine.show_dialogue("I'm going to give you a script. Read the comments which explain what it does and run it when you are ready.", callback = callback),
        lambda callback: engine.show_dialogue("Don't worry if you don't get it right away! There will be plenty of practise!", callback = callback),
        lambda callback: engine.clear_scripter(callback = callback),
        lambda callback: engine.insert_to_scripter("#This script will make you move east 17 steps.\n", callback = callback),
        lambda callback: engine.insert_to_scripter("count = 0 #Here we are making a new variable called 'count'\n\n", callback = callback),
        lambda callback: engine.insert_to_scripter("#We are saying 'do everything in the loop, while count is less than 17'\n", callback = callback),
        lambda callback: engine.insert_to_scripter("while count < 17:\n", callback = callback),
        lambda callback: engine.insert_to_scripter("  move_east() # Move yourself east, by one step!\n", callback = callback),
        lambda callback: engine.insert_to_scripter("  count = count + 1 # Here we are saying 'take count and increase the number in it by 1'\n", callback = callback),
        lambda callback: engine.insert_to_scripter("  print(count) # Print out the value of count into the PyConsole.'\n", callback = callback),
        lambda callback: engine.insert_to_scripter("  #Now, we are at the end of the loop, the program will check if count is still less than 17\n", callback = callback),
        lambda callback: engine.insert_to_scripter("  #If it is, it will run the loop again\n", callback = callback),
        lambda callback: engine.insert_to_scripter("  #Otherwise, it will carry on running the program!\n", callback = callback),
        lambda callback: engine.insert_to_scripter("#Like with functions, we use 'indenting' to say what is in our loop and outside of it\n", callback = callback),
        lambda callback: engine.insert_to_scripter('print("I have just finished walking!")', callback = callback),
        lambda callback: engine.show_dialogue("Don't forget that you can zoom in and out using the '+' and '-' buttons in the bottom left-hand corner of the window!", callback = callback),
        lambda callback: myla.follow(player_one, callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback)
    ])


def myla_tells_loop_modify_action(player_object):
    myla_tells_loop_modify.player_walked_on = lambda player_object: None
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: player_one.wait(0.3, callback = callback),
        lambda callback: myla.move_east(callback = callback),
        lambda callback: myla.face_south(callback = callback),
        lambda callback: player_one.face_north(callback = callback),
        lambda callback: engine.show_dialogue("You can change the script now " + engine.get_player_name() + ".", callback = callback),
        lambda callback: engine.show_dialogue("We want to move 30 steps to the South.", callback = callback),
        lambda callback: engine.show_dialogue("If you get stuck, just speak to me by pressing 'Enter' and I will help you out!", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback)
    ])

def myla_give_vine_loop_action(player_object):
    myla_give_vine_loop.player_walked_on = lambda player_object: None
    player_data.save_checkpoint("myla_give_vine_loop")
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("This is another section where a loop is useful.", callback = callback),
        lambda callback: engine.show_dialogue(engine.get_player_name() + ", try making a loop to cut down all these vines while you walk north.", callback = callback),
        lambda callback: engine.show_dialogue("If you can't think of a way to do it, speak to me and I will help out.", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback)
    ])


def myla_notice_bog_water_action(player_object):
    player_data.save_checkpoint("myla_notice_bog_water")
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("Myla: There's lots of crocodiles here making me feel uncomfortable.", callback = callback),
        lambda callback: engine.show_dialogue("Lets head out as quickly as we can.", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback)
    ])

#end function definitions
#assign functions to checkpoints
myla_explain_cut.player_walked_on = myla_explain_cut_action
myla_explain_whetstone.player_walked_on = myla_explain_whetstone_action
myla_explain_crocodiles.player_walked_on = myla_explain_crocodiles_action
myla_explain_basic_move.player_walked_on = myla_explain_basic_move_action
myla_give_simple_loop.player_walked_on = myla_give_simple_loop_action
myla_tells_loop_modify.player_walked_on = myla_tells_loop_modify_action
myla_give_vine_loop.player_walked_on = myla_give_vine_loop_action
myla_notice_bog_water.player_walked_on = myla_notice_bog_water_action
#end function assignment

#setting the player's starting position
if player_data.previous_exit_is(world_name, level_name = level_name, map_name = map_name, info = "myla_explain_cut"):
    x, y = myla_explain_cut.get_position()
    player_one.move_to((x, y), callback = lambda: myla_explain_cut_action(player_one))
    myla.set_solidity(False, callback = lambda: myla.move_to((x + 1, y), callback = lambda: myla.follow(player_one)))
elif player_data.previous_exit_is(world_name, level_name = level_name, map_name = map_name, info = "myla_explain_whetstone"):
    myla_explain_cut.player_walked_on = lambda player_object: None
    x, y = myla_explain_whetstone.get_position()
    player_one.move_to((x, y), callback = lambda: myla_explain_whetstone_action(player_one))
    myla.set_solidity(False, callback = lambda: myla.move_to((x, y-1), callback = lambda: myla.follow(player_one)))
elif player_data.previous_exit_is(world_name, level_name = level_name, map_name = map_name, info = "myla_explain_crocodiles"):
    myla_explain_cut.player_walked_on = lambda player_object: None
    myla_explain_whetstone.player_walked_on = lambda player_object: None
    x, y = myla_explain_crocodiles.get_position()
    player_one.move_to((x, y), callback = lambda: myla_explain_crocodiles_action(player_one))
    myla.set_solidity(False, callback = lambda: myla.move_to((x+1, y), callback = lambda: myla.follow(player_one)))
elif player_data.previous_exit_is(world_name, level_name = level_name, map_name = map_name, info = "myla_explain_basic_move"):
    myla_explain_cut.player_walked_on = lambda player_object: None
    myla_explain_whetstone.player_walked_on = lambda player_object: None
    myla_explain_crocodiles.player_walked_on = myla_go_in_bag
    x, y = myla_explain_basic_move.get_position()
    player_one.move_to((x, y), callback = lambda: myla_explain_basic_move_action(player_one))
    myla.set_solidity(False, callback = lambda: myla.move_to((x, y), callback = lambda: myla.follow(player_one)))
elif player_data.previous_exit_is(world_name, level_name = level_name, map_name = map_name, info = "myla_give_simple_loop"):
    myla_explain_cut.player_walked_on = lambda player_object: None
    myla_explain_whetstone.player_walked_on = lambda player_object: None
    myla_explain_crocodiles.player_walked_on = myla_go_in_bag
    myla_explain_basic_move.player_walked_on = myla_go_in_bag
    x, y = myla_give_simple_loop.get_position()
    player_one.move_to((x, y), callback = lambda: myla_give_simple_loop_action(player_one))
    myla.set_solidity(False, callback = lambda: myla.move_to((x - 2, y + 1), callback = lambda: myla.follow(player_one)))
elif player_data.previous_exit_is(world_name, level_name = level_name, map_name = map_name, info = "myla_give_vine_loop"):
    myla_explain_cut.player_walked_on = lambda player_object: None
    myla_explain_whetstone.player_walked_on = lambda player_object: None
    myla_explain_crocodiles.player_walked_on = myla_go_in_bag
    myla_explain_basic_move.player_walked_on = myla_go_in_bag
    myla_give_simple_loop.player_walked_on = lambda player_object: None
    myla_tells_loop_modify.player_walked_on = lambda player_object: None
    x, y = myla_give_vine_loop.get_position()
    player_one.move_to((x, y), callback = lambda: myla_give_vine_loop_action(player_one))
    myla.set_solidity(False, callback = lambda: myla.move_to((x, y-1), callback = lambda: myla.follow(player_one)))
elif player_data.previous_exit_is(world_name, level_name = level_name, map_name = map_name, info = "myla_notice_bog_water"):
    myla_explain_cut.player_walked_on = lambda player_object: None
    myla_explain_whetstone.player_walked_on = lambda player_object: None
    myla_explain_crocodiles.player_walked_on = myla_go_in_bag
    myla_explain_basic_move.player_walked_on = myla_go_in_bag
    myla_give_simple_loop.player_walked_on = lambda player_object: None
    myla_tells_loop_modify.player_walked_on = lambda player_object: None
    myla_give_vine_loop.player_walked_on = lambda player_object: None
    x, y = myla_notice_bog_water.get_position()
    player_one.move_to((x, y), callback = lambda: myla_notice_bog_water_action(player_one))
    myla.set_solidity(False, callback = lambda: myla.move_to((x + 1, y), callback = lambda: myla.follow(player_one)))
else:
    player_one.move_south()
    myla_start_position = exit_to_start.get_position()
    myla.set_solidity(False, callback = lambda: myla.move_to(myla_start_position, callback = lambda: myla.follow(player_one)))
#end setting the player's starting position

def go_to_start(player_object):
    player_data.save_and_exit("/world1")

def go_to_world(player_object):
    player_data.complete_level_and_save()
    player_data.save_and_exit("/world1")

exit_to_world.player_walked_on = go_to_world
exit_to_start.player_walked_on = go_to_start

bog_sign.set_message("Welcome to the bogs! The home of stinky water...")

#TODO: Use a method in engine to get all the vines in a level.
vine_list = [
    vine0,
    vine1,
    vine2,
    vine3,
    vine4,
    vine5,
    vine6,
    vine6,
    vine7,
    vine8,
    vine9,
    vine10,
    vine11,
    vine12,
    vine13,
    vine14,
    vine15
]

whetstone1.prepare(2, vine_list)
whetstone2.prepare(11, vine_list)


player_one.focus()
engine.play_music("world_1_jungle")
croc_guard_totem.face_north()

#Making it so that crocs can kill you, HACK FOR BEFORE CROCS CAN CREATE THEIR OWN LISTENERS, WILL BE IMPROVED!!!!

b2_croc = [
    croc_face_vine_left,
    croc_face_vine_right,
    croc_bottom_0,
    croc_bottom_1,
    croc_bottom_2,
    croc_bottom_3,
    croc_bottom_4,
    croc_bottom_5,
    croc_bottom_6,
    croc_bottom_7,
    croc_guard_totem
]

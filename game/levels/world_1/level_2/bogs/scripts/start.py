#commence save-data set-up
world_name = "world_1"
level_name = "level_2"
map_name = "bogs"

player_data.load(engine.get_player_name())
player_data.set_map(world_name, level_name = level_name, map_name = map_name) #change the map and save that the map has changed
#end save-data set-up

#defining verious functions

def myla_explain_cut_action(player_object):
    player_one.set_cuts_left(2)
    myla_explain_cut.player_walked_on = lambda player_object: None
    player_data.save_checkpoint("myla_explain_cut_action")

    def myla_ask_player_if_there_is_way(callback):
        engine.show_dialogue_with_options(
            "Aren't you gonna ask me if there is a way?",
            {
                "Yes" : lambda: engine.show_dialogue("Why thank you, you can use your *ahem*, I mean MY PyScripter!", callback = callback),
                "No": lambda: engine.show_dialogue("Argh ok you win. You can use the PyScripter.", callback = callback)
            }
        )

    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: player_one.face_east(callback = callback),
        lambda callback: engine.show_dialogue(engine.get_player_name() + ", there's a vine in the way over there.", callback = callback),
        lambda callback: engine.show_dialogue("What's that, I hear you ask? How could we possibly make it through?", callback = callback),
        lambda callback: engine.show_dialogue("If only there was a way...", callback = callback),
        lambda callback: engine.show_dialogue("...", callback = callback),
        myla_ask_player_if_there_is_way,
        lambda callback: engine.show_dialogue("I'm going to give you my knife so that you can cut down vines.", callback = callback),
        lambda callback: engine.show_dialogue("The PyScripter let's you control objects from the real world. So you can use it to cut down vines!", callback = callback),
        lambda callback: engine.show_dialogue("I have written a small script for you and put it into your PyScripter...", callback = callback),
        lambda callback: engine.show_dialogue("Stand next to a vine and hit the 'Run' button (or you can use the spacebar to run scripts)!", callback = callback),
        lambda callback: engine.clear_scripter(callback = callback),
        lambda callback: engine.insert_to_scripter("cut()\n", callback = callback),
        lambda callback: engine.insert_to_scripter('print("You have " + str(get_cuts_left()) + " cut(s) left.")', callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback)
    ])


def myla_explain_whetstone_action(player_object):
    myla_explain_whetstone.player_walked_on = lambda player_object: None
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: myla.stop_follow(callback = callback),
        lambda callback: myla.set_solidity(True, callback = callback),
        lambda callback: myla.wait(0.3, callback = callback),
        lambda callback: myla.move_east(callback = callback),
        lambda callback: myla.face_north(callback = callback),
        lambda callback: player_one.face_south(callback = callback),
        lambda callback: engine.show_dialogue("You might have noticed that in the console the number of 'cuts' you have left decreased when you cut the vines.", callback = callback),
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
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
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
        lambda callback: engine.show_dialogue("You can also use the PyScripter to move about " + engine.get_player_name() + ".", callback = callback),
        lambda callback: engine.show_dialogue("I'm just gonna put some movement functions into your PyScripter...", callback = callback),
        lambda callback: engine.clear_scripter(callback = callback),
        lambda callback: engine.insert_to_scripter("print(\"Starting Movement\") \n", callback = callback),
        lambda callback: engine.insert_to_scripter("move_west()\n", callback = callback),
        lambda callback: engine.insert_to_scripter("move_south()\n", callback = callback),
        lambda callback: engine.insert_to_scripter("move_west()\n", callback = callback),
        lambda callback: engine.insert_to_scripter("move_south()\n", callback = callback),
        lambda callback: engine.insert_to_scripter("move_west()\n", callback = callback),
        lambda callback: engine.insert_to_scripter("move_south()\n", callback = callback),
        lambda callback: engine.insert_to_scripter("move_south()\n", callback = callback),
        lambda callback: engine.insert_to_scripter("move_south()\n", callback = callback),
        lambda callback: engine.insert_to_scripter("move_west()\n", callback = callback),
        lambda callback: engine.insert_to_scripter("move_north()\n", callback = callback),
        lambda callback: engine.insert_to_scripter("move_east()\n", callback = callback),
        lambda callback: engine.insert_to_scripter("move_south()\n", callback = callback),
        lambda callback: engine.insert_to_scripter("move_south()\n", callback = callback),
        lambda callback: engine.insert_to_scripter("move_south()\n", callback = callback),
        lambda callback: engine.insert_to_scripter("move_east()\n", callback = callback),
        lambda callback: engine.insert_to_scripter("print(\"Stopped Movement\") \n", callback = callback),
        lambda callback: engine.show_dialogue("A function is something you can use to 'do' something, they always end with a pair of brackets ().", callback = callback),
        lambda callback: engine.show_dialogue("cut() is also a function.", callback = callback),
        lambda callback: engine.show_dialogue("Moving by programming might seem a bit pointless now when you can just use the arrow keys...", callback = callback),
        lambda callback: engine.show_dialogue("..BUT play around with it anyway and give it a try.", callback = callback),
        lambda callback: engine.show_dialogue("I will show you the true power of scripting in a sec, teehee...", callback = callback),
        lambda callback: myla.follow(player_one, callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback)
    ])

def myla_give_simple_loop_action(player_object):
    myla_give_simple_loop.player_walked_on = lambda player_object: None
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: myla.set_solidity(True, callback = callback),
        lambda callback: myla.stop_follow(callback = callback),
        lambda callback: myla.move_south(callback = callback),
        lambda callback: myla.move_east(callback = callback),
        lambda callback: player_one.face_west(callback = callback),
        lambda callback: engine.show_dialogue("I remember coming here before, this path is really, really, really, really long...", callback = callback),
        lambda callback: engine.show_dialogue("Maybe not that long, but long enough to be BORING.", callback = callback),
        lambda callback: engine.show_dialogue("I'm just gonna give you another script...", callback = callback),
        lambda callback: engine.clear_scripter(callback = callback),
        lambda callback: engine.insert_to_scripter("for i in range(17):\n", callback = callback),
        lambda callback: engine.insert_to_scripter("  move_east()\n", callback = callback),
        lambda callback: engine.insert_to_scripter('print("Script Finished!")\n', callback = callback),
        lambda callback: engine.show_dialogue("Aha! There we go. This script will make you move east 20 times!", callback = callback),
        lambda callback: engine.show_dialogue("The PyScripter has some awesome technology that lets you speed up time.", callback = callback),
        lambda callback: engine.show_dialogue("Look at the bottom-right of your PyScripter " + engine.get_player_name() + " and you will see a 'Speed' button.", callback = callback),
        lambda callback: engine.show_dialogue("Give it a click so that it says 'Speed: Fast' (or use the shift key).", callback = callback),
        lambda callback: engine.show_dialogue("Then run your script!", callback = callback),
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
        lambda callback: engine.show_dialogue("Fast right?", callback = callback),
        lambda callback: engine.show_dialogue("If I remember correctly, the next bit is 30 steps to the south.", callback = callback),
        lambda callback: engine.show_dialogue("Your turn to change the script!!!", callback = callback),
        lambda callback: engine.clear_scripter(callback = callback),
        lambda callback: engine.insert_to_scripter("#You probably already forgot how many steps\n", callback = callback),
        lambda callback: engine.insert_to_scripter("#and in which direction we are going.\n", callback = callback),
        lambda callback: engine.insert_to_scripter("#Luckily, Myla to the rescue!!!\n", callback = callback),
        lambda callback: engine.insert_to_scripter("#btw this is something called a comment\n", callback = callback),
        lambda callback: engine.insert_to_scripter("#everything after a '#' on a line in your program will be ignored.\n", callback = callback),
        lambda callback: engine.insert_to_scripter("#socool #awesome #pyscripter\n", callback = callback),
        lambda callback: engine.insert_to_scripter("#We are heading 30 steps south.\n", callback = callback),
        lambda callback: engine.insert_to_scripter("for i in range(17): # change the number of steps we will be moving from 17 to 30\n", callback = callback),
        lambda callback: engine.insert_to_scripter("  move_east()       # change this to move_south()\n", callback = callback),
        lambda callback: engine.insert_to_scripter('print("Script Finished!")\n', callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback)
    ])

def myla_give_vine_loop_action(player_object):
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("The PyScripter is really useful for solving repetative tasks, quickly.", callback = callback),
        lambda callback: engine.show_dialogue(engine.get_player_name() + " I'm just going to give you a new script to cut down all these vines.", callback = callback),
        lambda callback: engine.clear_scripter(callback = callback),
        lambda callback: engine.insert_to_scripter('""" This is another type of comment,\n', callback = callback),
        lambda callback: engine.insert_to_scripter('it lasts for multiple lines!\n', callback = callback),
        lambda callback: engine.insert_to_scripter('"""\n', callback = callback),
        lambda callback: engine.insert_to_scripter("for i in range(11):\n", callback = callback),
        lambda callback: engine.insert_to_scripter("  move_north()\n", callback = callback),
        lambda callback: engine.insert_to_scripter("  cut()\n", callback = callback),
        lambda callback: engine.insert_to_scripter("  move_north()", callback = callback),
        lambda callback: engine.insert_to_scripter('print("Cut all the vines, yeah!")\n', callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback)
    ])


def myla_notice_bog_water_action(player_object):
    engine.run_callback_list_sequence([
        lambda callback: player_one.set_busy(True, callback = callback),
        lambda callback: engine.show_dialogue("Here the player will be able to get bog-water and proceed to the next level with Myla...", callback = callback),
        lambda callback: player_one.set_busy(False, callback = callback)
    ])

#end function definitions

#setting the player's starting position
if player_data.previous_exit_is("world_1", level_name = "level_2", map_name = "start"):
    player_one.move_south()
    myla_start_position = exit_to_start.get_position()
    myla.set_solidity(False, callback = lambda: myla.move_to(myla_start_position, callback = lambda: myla.follow(player_one)))
elif player_data.previous_exit_is(world_name, level_name = level_name, map_name = map_name, info = "myla_explain_cut"):
    x, y = myla_explain_cut.get_position()
    player_one.move_to((x, y), callback = lambda: myla_explain_cut_action(player_one))
    myla.set_solidity(False, callback = lambda: myla.move_to((x + 1, y), callback = lambda: myla.follow(player_one)))

#end setting the player's starting position

def go_to_start(player_object):
    player_data.save_and_exit("/world_1/level_2/start")

def go_to_world(player_object):
    player_data.complete_level_and_save()
    player_data.save_and_exit("/world_1")

exit_to_world.player_walked_on = go_to_world


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

myla_explain_cut.player_walked_on = myla_explain_cut_action
myla_explain_whetstone.player_walked_on = myla_explain_whetstone_action
myla_explain_crocodiles.player_walked_on = myla_explain_crocodiles_action
myla_explain_basic_move.player_walked_on = myla_explain_basic_move_action
myla_give_simple_loop.player_walked_on = myla_give_simple_loop_action
myla_tells_loop_modify.player_walked_on = myla_tells_loop_modify_action
myla_give_vine_loop.player_walked_on = myla_give_vine_loop_action
myla_notice_bog_water.player_walked_on = myla_notice_bog_water_action


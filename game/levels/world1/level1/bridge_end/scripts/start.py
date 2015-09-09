#commence save-data set-up
world_name = "world1"
level_name = "level1"
map_name = "bridge_end"

engine.update_world_text("1")
engine.update_level_text("2")

player_data.load(engine.get_player_name())
player_data.set_map(world_name, level_name = level_name, map_name = map_name) #change the map and save that the map has changed
#end save-data set-up

#defining the possible states of the level
pyscripter_state_not_found = 0
pyscripter_state_found = 1
#end state definitions

#setting the player's starting position
if player_data.previous_exit_is("world1", level_name = "level1", map_name = "bridge"):
    player_one.move_east()
    myla_start_position = (3, 10)
#end setting the player's starting position

def go_to_bridge(player_object):
    player_data.save_and_exit("/world1/level1/bridge")

def go_to_world(player_object):
    player_data.complete_level_and_save()
    player_data.save_and_exit("/world1")

exit_to_bridge.player_walked_on = go_to_bridge
exit_to_world.player_walked_on = go_to_world

pyscripter_state = player_data.get_level_state("pyscripter_state")
if pyscripter_state == pyscripter_state_not_found:
    def pyscripter_player_action(player_object):
        engine.run_callback_list_sequence([
            lambda callback: player_one.set_busy(True, callback = callback),
            lambda callback: pyscripter.set_visible(False, callback = callback),
            lambda callback: pyscripter.set_solidity(False, callback = callback),
            lambda callback: pyscripter.move_to((0, 0), callback = callback),
            lambda callback: engine.show_dialogue(engine.get_player_name() + " picked up the PyScripter", callback = callback),
            lambda callback: engine.disable_py_scripter(callback = callback),
            lambda callback: engine.clear_scripter(callback = callback),
            lambda callback: engine.show_py_scripter(callback = callback),
            lambda callback: engine.show_dialogue("Annoying Croc: Stop right there!", callback = callback),
            lambda callback: annoying_croc.move_north(callback = callback),
            lambda callback: annoying_croc.move_north(callback = callback),
            lambda callback: annoying_croc.move_north(callback = callback),
            lambda callback: annoying_croc.move_north(callback = callback),
            lambda callback: annoying_croc.move_west(callback = callback),
            lambda callback: annoying_croc.move_west(callback = callback),
            lambda callback: annoying_croc.move_west(callback = callback),
            lambda callback: annoying_croc.move_west(callback = callback),
            lambda callback: annoying_croc.move_west(callback = callback),
            lambda callback: annoying_croc.move_west(callback = callback),
            lambda callback: annoying_croc.move_west(callback = callback),
            lambda callback: annoying_croc.move_north(callback = callback),
            lambda callback: annoying_croc.move_west(callback = callback),
            lambda callback: annoying_croc.move_west(callback = callback),
            lambda callback: engine.show_dialogue("You've found what I was looking for, you've been working with the monkey all along you punk!", callback = callback),
            lambda callback: engine.show_dialogue_with_options("Are you gonna hand the PyScripter over or not?", { "Yes": callback, "No" : callback}),
            lambda callback: engine.show_dialogue("I was gonna take it off you anyway, give it 'ere!", callback = callback),
            lambda callback: player_one.face_north(callback = callback),
            lambda callback: annoying_croc.face_north(callback = callback),
            lambda callback: engine.play_music("world_1_myla", callback = callback),
            lambda callback: engine.show_dialogue("???: Wait you lousy croc!", callback = callback),
            lambda callback: engine.show_dialogue("Monkey surprise attack!", callback = callback),
            lambda callback: myla.face_south(callback = callback),
            lambda callback: myla.set_solidity(False, callback = callback),
            lambda callback: myla.start_animating(callback = callback),
            lambda callback: myla.move_to(annoying_croc.get_position(), time = 0.8, callback = callback),
            lambda callback: myla.stop_animating(callback = callback),
            lambda callback: annoying_croc.face_west(callback = callback),
            lambda callback: annoying_croc.start_animating(callback = callback),
            lambda callback: annoying_croc.move_by((1, 0), time = 0.5, callback = callback),
            lambda callback: annoying_croc.stop_animating(callback = callback),
            lambda callback: myla.face_east(callback = callback),
            lambda callback: player_one.wait(0.5, callback = callback),
            lambda callback: player_one.face_east(callback = callback),
            lambda callback: engine.show_dialogue("Annoying Croc: Ow!", callback = callback),
            lambda callback: engine.show_dialogue("I might be outnumbered by the two of you now, but I will put the lads and lasses on patrol to catch you.", callback = callback),
            lambda callback: engine.show_dialogue("I will tell them not to bite you...", callback = callback),
            lambda callback: engine.show_dialogue("... too hard. Hehehehehehe.", callback = callback),
            lambda callback: annoying_croc.face_east(callback = callback),
            lambda callback: annoying_croc.move_east(callback = callback),
            lambda callback: annoying_croc.move_south(callback = callback),
            lambda callback: annoying_croc.move_east(callback = callback),
            lambda callback: annoying_croc.move_east(callback = callback),
            lambda callback: annoying_croc.move_east(callback = callback),
            lambda callback: annoying_croc.wait(0.5, callback = callback),
            lambda callback: annoying_croc.face_west(callback = callback),
            lambda callback: engine.show_dialogue("Annoying Croc: And Myla...", callback = callback),
            lambda callback: engine.show_dialogue("... your mother is very dissapointed.", callback = callback),
            lambda callback: annoying_croc.face_east(callback = callback),
            lambda callback: annoying_croc.start_animating(callback = callback),
            lambda callback: engine.show_dialogue("Mwahahahahaah!", callback = callback),
            lambda callback: annoying_croc.stop_animating(callback = callback),
            lambda callback: annoying_croc.move_south(callback = callback),
            lambda callback: annoying_croc.move_east(callback = callback),
            lambda callback: annoying_croc.move_south(callback = callback),
            lambda callback: annoying_croc.move_east(callback = callback),
            lambda callback: annoying_croc.move_south(callback = callback),
            lambda callback: annoying_croc.move_east(callback = callback),
            lambda callback: annoying_croc.move_south(callback = callback),
            lambda callback: annoying_croc.set_visible(False, callback = callback),
            lambda callback: annoying_croc.set_solidity(False, callback = callback),
            lambda callback: myla.face_west(callback = callback),
            lambda callback: engine.show_dialogue("???: Thank you so much for helping me out back there!", callback = callback),
            lambda callback: engine.show_dialogue("I think if you hadn't come along I don't know what I would have done.", callback = callback),
            lambda callback: engine.show_dialogue("The green one you just had the pleasure of meeting is Carl.", callback = callback),
            lambda callback: engine.show_dialogue("By the way I'm Myla, as you heard I guess? Oh sorry! You are?", callback = callback),
            lambda callback: engine.show_dialogue("...", callback = callback),
            lambda callback: engine.show_dialogue("Nice to meet you " + engine.get_player_name() + "!", callback = callback),
            lambda callback: engine.show_dialogue("It seems that you have found my PyScripter, this is an early prototype, there aren't many around ...", callback = callback),
            lambda callback: engine.show_dialogue("Anyway, I will let you keep it for a while if you help me get to the other side of the jungle. I could do with some help!", callback = callback),
            lambda callback: engine.show_dialogue("Let's head to the bog, I will teach you the basics there!", callback = callback),
            lambda callback: engine.play_music("world_1_jungle", callback = callback),
            lambda callback: myla.follow(player_one, callback = callback),
            lambda callback: player_one.set_busy(False, callback = callback)
        ], save_pyscripter_state)

    def save_pyscripter_state():
        player_data.set_level_state("pyscripter_state", pyscripter_state_found)
        player_data.unlock_pyscripter()
        player_data.save()

    pyscripter.player_action = pyscripter_player_action

elif pyscripter_state == pyscripter_state_found:
    pyscripter.set_visible(False)
    pyscripter.set_solidity(False, callback = lambda: pyscripter.move_to((0,0)))
    annoying_croc.set_visible(False)
    annoying_croc.set_solidity(False)
    myla.move_to(myla_start_position)
    myla.follow(player_one)

player_one.focus()
engine.play_music("world_1_jungle")

bog_sign.set_message("(west) rossumberg (south) bog")

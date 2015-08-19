player_one.focus()

engine.play_music("calm")
engine.set_ui_colours((200,255,200),(215,255,215)) #TODO: save these colours in the config.

""" Heidi introducing herself and walking over to the player """
heidi_introduction_sequence = [
    lambda callback: player_one.set_busy(True, callback = callback),
    lambda callback: player_one.face_south(callback = callback),
    lambda callback: heidi.face_west(callback = callback),
    lambda callback: engine.show_dialogue("Heidi: %player_name%!!!!", callback = callback),
    lambda callback: heidi.move_west(callback = callback),
    lambda callback: heidi.move_west(callback = callback),
    lambda callback: heidi.move_west(callback = callback),
    lambda callback: heidi.move_west(callback = callback),
    lambda callback: heidi.move_north(callback = callback),
    lambda callback: heidi.move_west(callback = callback),
    lambda callback: heidi.move_west(callback = callback),
    lambda callback: player_one.face_east(callback = callback),
]

""" Run the introdoction dialogue, and end it with asking the player about wether they know about the "big day today", (the prank), and if they say yes, asking what they think it is."""
engine.run_callback_list_sequence(
    heidi_introduction_sequence,
    lambda: engine.show_dialogue_with_options(
        "Today is the big day! You did remember right?",
        {
            "Yes": lambda: engine.show_dialogue_with_options(
                "Ok, so what are what are we doing today then?",
                {
                    "Ummmmm..."  : lambda: engine.run_callback_list_sequence(heidi_explain_prank_sequence),
                    "Ahhhhhhh...": lambda: engine.run_callback_list_sequence(heidi_explain_prank_sequence)
                }
            ),
            "No" : lambda: engine.run_callback_list_sequence(heidi_explain_prank_sequence)
        }
    )
)

""" Heidi explains to the player that today is the day of the big prank, and that they need to head to the merchant for materials """
heidi_explain_prank_sequence = [
    lambda callback: engine.show_dialogue("You forgot you goon!", callback = callback),
    lambda callback: engine.show_dialogue("Today's the day of our big prank, remember?", callback = callback),
    lambda callback: engine.show_dialogue("Hehehehehehehehe...", callback = callback),
    lambda callback: heidi.wait(0.5, callback = callback),
    lambda callback: heidi.face_south(callback = callback),
    lambda callback: heidi.wait(0.5, callback = callback),
    lambda callback: engine.show_dialogue("Hahahahahahahahahahaha!", callback = callback),
    lambda callback: heidi.start_animating(callback = callback),
    lambda callback: engine.show_dialogue("MWAHAHAHAHAHAHAHAHAHAHAHAHA!", callback = callback),
    lambda callback: heidi.wait(0.5, callback = callback),
    lambda callback: heidi.stop_animating(callback = callback),
    lambda callback: heidi.wait(0.5, callback = callback),
    lambda callback: heidi.face_west(callback = callback),
    lambda callback: heidi.wait(0.5, callback = callback),
    lambda callback: engine.show_dialogue("Oops, sorry, got a bit carried away there. Anyway, let's head east to the merchant! He has all the stuff we need, then you can go to the bog to get the dirty bog water!", callback = callback),
    lambda callback: player_one.set_busy(False, callback = callback),
    lambda callback: heidi_player_reminder(100.0, callback = callback)
]


def heidi_player_reminder(initial_wait, callback = lambda: None):
    heidi.wait(
        initial_wait,
        callback = lambda: engine.show_dialogue(
            "Heidi: Remember, the merchant is east.",
            callback = lambda: heidi_player_reminder(initial_wait * 3)
        )
    )
    callback()



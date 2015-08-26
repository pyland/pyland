import sys
import os
sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/../../../characters')
#sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/../../../game_object')
#from game_object import GameObject
from character import Character #TODO: it should import from here, but some kind of definitions with the movement functions are screwing around with follow_path, causing the game to freeze.

import random

""" A brief description of the class will go here.
The auto-generated comment produced by the script should also mention where to get a list of the api
and which built-in variables exist already.
"""
class Myla(Character):

    def initialise(self):
        super().initialise()

    def player_action(self, player_object):
        """ This is the method that is run if the player presses the action key while facing a character.
        
        TODO: move this to character.py and make it print an explenation on how to override this by default.
        """
        pass

    def yelled_at(self, player):
        player_x, player_y = player.get_position()
        self_x, self_y = self.get_position()
        if not self.is_moving():
            if player_y == self_y:
                if player_x < self_x and player.is_facing_east():
                    self.myla_yell(player)
                elif player_x > self_x and player.is_facing_west():
                    self.myla_yell(player)
                return
            if player_x == self_x:
                if player_y < self_y and player.is_facing_north():
                    self.myla_yell(player)
                elif player_y > self_y and player.is_facing_south():
                    self.myla_yell(player)
                return

        
    def myla_yell(self, player):
        #TODO: make this work like 
        engine = self.get_engine()
        engine.run_callback_list_sequence([
            lambda callback: player.set_busy(True, callback = callback),
            lambda callback: engine.show_dialogue("Myla: AHH, STOP YELLING AT ME!", callback = callback),
            lambda callback: player.set_busy(False, callback =callback)
        ])

    def on_cut(self, callback = lambda: None):
        engine = self.get_engine()
        cut_responses = [
            "Oi, don't even think about that.",
            "I still have another knife, so don't do that",
            "I'm gonna cut you back if you try that.",
            "That isn't a good idea, no matter which way you slice it.",
            "I would take a stab at the task ahead, not at me!",
            "That knife is meant for vines, get the point already?",
            "If you do that... well... it was knife to know you", #TODO: make this a monty pun
        ]
        engine.run_callback_list_sequence([
            lambda callback: engine.show_dialogue(random.choice(cut_responses), callback = callback)
        ], callback = callback)
        #player.halt_script() #TODO, make it so that this isn't needed, 
        return False










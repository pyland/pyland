import sys
import os
import random

sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/..')
from npc import NPC

class Generic(NPC):

    __dialogue_list = ["Hello there."]
    __in_conversation = False

    def initialise(self):
        super().initialise()

    def player_action(self, player_object):
        """ This is the method that is run if the player presses the action key while facing the generic NPC.

        By default it shows the list of strings that have been set using set_dialogue_list

        Parameters
        ----------
        player_object : Player 
            The player that interacts with the generic NPC
        """
        engine = self.get_engine()
        dialogue_actions = []
        dialogue_actions.append(lambda callback: player_object.set_busy(True, callback = callback))
        dialogue_actions.append(lambda callback: self.__set_in_conversation(True, callback = callback))
        dialogue_actions.append(lambda callback: self.turn_to_face(player_object, callback = callback))
        for dialogue in self.__dialogue_list:
            def fun(dialogue): #Done so that this works
                return lambda callback: engine.show_dialogue(dialogue, callback = callback)
            dialogue_actions.append(fun(dialogue))
        dialogue_actions.append(lambda callback: self.__set_in_conversation(False, callback = callback))
        dialogue_actions.append(lambda callback: player_object.set_busy(False, callback = callback))
        engine.run_callback_list_sequence(dialogue_actions)

    def set_dialogue_list(self, dialogue_list):
        """ Sets the NPC dialogue to dialogue_list. 

        The dialogue_list is run whenever the player talks to the NPC

        Parameters
        ----------
        dialogue_list : list of dialogues 
            The list of dialogues (functions) to be run in order.
        """
        self.__dialogue_list = dialogue_list
    
    def __set_in_conversation(self, value, callback):
        self.__in_conversation = value
        self.get_engine().add_event(callback)

    def rand_explore(self):
        """ The NPC follows a random path forever
        """
        if self.__in_conversation:
            return self.wait(5.0, callback = self.rand_explore)
        rand = random.randint(0, 60)
        if rand==0:
            return self.move_north(callback = self.rand_explore)
        elif rand==1:
            return self.move_south(callback = self.rand_explore)
        elif rand==2:
            return self.move_east(callback = self.rand_explore)
        elif rand==3:
            return self.move_west(callback = self.rand_explore)
        elif rand==4:
            return self.face_north(callback = self.rand_explore)
        elif rand==5:
            return self.face_south(callback = self.rand_explore)
        elif rand==6:
            return self.face_east(callback = self.rand_explore)
        elif rand==7:
            return self.face_west(callback = self.rand_explore)
        else:
            return self.wait(0.3, callback = self.rand_explore)





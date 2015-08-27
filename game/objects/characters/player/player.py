#Python modules
import operator
import os
import sys

#Custom modules
sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/../../../script_running')
import scriptrunner
from script_state_container import ScriptStateContainer

sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/../../characters')
from character import Character

sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/../../properties/bagable')
from bagable import Bagable

"""
As the Character is in the characters folder.
"""


""" A brief description of the class will go here.
The auto-generated comment produced by the script should also mention where to get a list of the api
and which built-in variables exist already.
"""
class Player(Character):

    __bag = []
    __focus_button_id = 0

    def initialise(self):
        """ An initialiser function.

        This function is called once all the neccesary set-up of the object has completed
        run when the object is created in-engine
        """
        super().initialise()
        engine = self.get_engine()

        def focus_func(func):
            """ Wraps functions so that they are only run if the player is the focus """
            return lambda: func() if self.is_focus() else lambda: None

        #register input callbacks to make character playable
        #register callbacks for running player scripts
        engine.register_input_callback(engine.INPUT_RUN, focus_func(self.run_script))
        engine.register_input_callback(engine.INPUT_HALT, focus_func(self.halt_script))

        #register callbacks for character movement
        engine.register_input_callback(engine.INPUT_UP, focus_func(self.__input_move_north))
        engine.register_input_callback(engine.INPUT_RIGHT, focus_func(self.__input_move_east))
        engine.register_input_callback(engine.INPUT_DOWN, focus_func(self.__input_move_south))
        engine.register_input_callback(engine.INPUT_LEFT, focus_func(self.__input_move_west))

        #register callback for talking to characters
        engine.register_input_callback(engine.INPUT_ACTION, focus_func(self.__trigger_action))

        #Get the correct image to the chosen for the sprite
        #####engine.print_terminal("Load in this image for the player head: "+"objects/"+super().get_file_location() + "/sprites/head")
        self.__focus_button_id = engine.add_button("gui/head/monkey", self.get_character_name(), self.focus)
        self.set_character_name(engine.get_player_name())

    """ public:
    Put the regular public methods you wish to use here.
    """

    def focus(self):
        """Override focus method for generic game_object, to update running button
        and update focused player button above
        """
        #self.__entity.focus()
        super().focus();
        engine = self.get_engine()
        if (self.is_running_script()):
            engine.set_running()
        else:
            engine.set_finished()
        #Update the currently focused player in the player heads at the top
        engine.set_cur_player(self.__focus_button_id)
        return

    def set_character_name(self, character_name):
        """Override set_character_name from character to
        update the player focus button with the new name
        """
        super().set_character_name(character_name)
        engine = self.get_engine()
        engine.update_player_name(character_name,self.__focus_button_id)

    #override ScriptStateContainer
    def get_script_name(self):
        return self.get_character_name()

    #override ScriptStateContainer---
    def set_script_name(self):
        self.set_character_name(self)

    def get_focus_button_id(self):
        return self.__focus_button_id

    """ ---- / All Code todo with running player scripts ---- """

    """ private:
    Put the private methods you wish to use here.
    """

    """ Override character move methods to trigger player events and stop the player from being walking on water (even when they are scripter)
    """

    def move_north(self, callback = lambda: None):
        engine = self.get_engine()
        x,y = self.get_position()
        def callback_wrap():
            self.__trigger_walk_on() #call walk-on triggers on objects player walks on
            engine.add_event(callback)
        if engine.get_tile_type((x,y+1)) != engine.TILE_TYPE_WATER: #stop the player from being Chris Angel
            super().move_north(callback_wrap)
        else:
            self.face_north()
            self.move_on_spot(callback)

    def move_east(self, callback = lambda: None):
        engine = self.get_engine()
        x,y = self.get_position()
        def callback_wrap():
            self.__trigger_walk_on() #call walk-on triggers on objects player walks on
            engine.add_event(callback)
        if engine.get_tile_type((x+1,y)) != engine.TILE_TYPE_WATER: #stop the player from being Chris Angel
            super().move_east(callback_wrap)
        else:
            self.face_east()
            self.move_on_spot(callback)

    def move_south(self, callback = lambda: None):
        engine = self.get_engine()
        x,y = self.get_position()
        def callback_wrap():
            self.__trigger_walk_on() #call walk-on triggers on objects player walks on
            engine.add_event(callback)
        if engine.get_tile_type((x,y-1)) != engine.TILE_TYPE_WATER: #stop the player from being Chris Angel
            super().move_south(callback_wrap)
        else:
            self.face_south()
            self.move_on_spot(callback)

    def move_west(self, callback = lambda: None):
        engine = self.get_engine()
        x,y = self.get_position()
        def callback_wrap():
            self.__trigger_walk_on() #call walk-on triggers on objects player walks on
            engine.add_event(callback)
        if engine.get_tile_type((x-1,y)) != engine.TILE_TYPE_WATER: #stop the player from being Chris Angel
            super().move_west(callback_wrap)
        else:
            self.face_west()
            self.move_on_spot(callback)

    def __input_move_north(self):
        if (not self.is_running_script()) and (not self.is_moving()) and (not self.is_busy()): #Check that a script isn't running
            if self.is_facing_north():
                self.move_north()
            else:
                self.wait(0.07, lambda: self.face_north() if not self.is_moving() else lambda: None) #this is done to allow the player character to face a different directino when tapped
        return

    def __input_move_east(self):
        if (not self.is_running_script()) and (not self.is_moving()) and (not self.is_busy()): #Check that a script isn't running
            if self.is_facing_east():
                self.move_east()
            else:
                self.wait(0.07, lambda: self.face_east() if not self.is_moving() else lambda: None)
        return

    def __input_move_south(self, callback = lambda: None):
        if (not self.is_running_script()) and (not self.is_moving()) and (not self.is_busy()): #Check that a script isn't running
            if self.is_facing_south():
                self.move_south()
            else:
                self.wait(0.07, lambda: self.face_south() if not self.is_moving() else lambda: None)
        return

    def __input_move_west(self, callback = lambda: None):
        if (not self.is_running_script()) and (not self.is_moving()) and (not self.is_busy()): #Check that a script isn't running
            if self.is_facing_west():
                self.move_west()
            else:
                self.wait(0.07, lambda: self.face_west() if not self.is_moving() else lambda: None)
        return

    def __trigger_action(self):
        if(not self.is_busy()):
            engine = self.get_engine()
            x, y = self.get_position()
            if self.is_facing_north():
                y += 1
            elif self.is_facing_east():
                x += 1
            elif self.is_facing_south():
                y += -1
            elif self.is_facing_west():
                x += -1

            game_objects = engine.get_objects_at((x, y))
            for game_object in game_objects:
                if(hasattr(game_object, "player_action")):
                    game_object.player_action(self)
        return


    def __trigger_walk_on(self):
        """ Triggers the walked-on functions for objects, objects which have a walked_on method will have those methods automatically called when they are walked on.

        Everytime the player finishes, walking, this is called. The player looks at all objects they are standing on and triggers their player_walked_one method if they have one.
        """
        engine = self.get_engine()
        position = self.get_position()
        game_objects = engine.get_objects_at(position)
        for game_object in game_objects:
            if(hasattr(game_object, "player_walked_on")):
                game_object.player_walked_on(self)
        return

    def add_to_bag(self, bag_item):
        pass

    def get_flag_message(self):
        message = "There is no flag here!"
        engine = self.get_engine()
        position = self.get_position()
        game_objects = engine.get_objects_at(position)
        for current_object in game_objects:
            if(hasattr(current_object, "get_message")):
                message = current_object.get_message()
        return message

    def kill(self):
        self.get_engine().restart_level()

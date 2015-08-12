#Python modules
import operator
import os
import ctypes #for sending exceptions to python threads!!!

#Custom modules
import scriptrunner


"""
In Python comments,
could define some standard which the C++ code can use to determine things about it handles
the python code
"""

"""
eg. We could be able to write:
"""
#__ import_object characters/Character
"""
The game code, at runtime, could recognise the "#__"
and replace it with:
"""
import sys
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

    __running_script = False
    __thread_id = 0
    __bag = None
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
            return lambda: func() if self.is_focus() else None

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
        engine.register_input_callback(engine.INPUT_TALK, focus_func(self.__trigger_action))

        #Make clicks be registered as callbacks
        #engine.register_input_callback(engine.INPUT_CLICK, focus_func(self.__focus))

        #Get the correct image to the chosen for the sprite
        self.__focus_button_id = engine.add_button("gui/head/monkey", self.get_character_name(), self.focus)

    """ game engine features (public)
    These are methods which the game engine will execute at the commented moments.
    This will all be autofilled by the creation script with super filled in to help
    prevent errors when it comes to this.
    """

    """ This method is run every frame before the graphics are displayed.
    You can put code here you want to run before every frame.
    You MUST but super().beforeFrameUpdate() for this to work. Otherwise this may lead
    to unexpected behaviour. MAY NOT BE NEEDED, may be able to do everything with callbacks!
    """
    #def before_frame_update(self):
        #super().before_frame_update()

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
        if (self.__running_script):
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
        self.__character_name = character_name
        engine = self.get_engine()
        engine.update_player_name(character_name,self.__focus_button_id)

    def test_display_bag(self):
        engine = self.get_engine()
        x, y = self.get_position()
        game_objects = engine.get_objects_at((x, y+1))
        for game_object in game_objects:
            if isinstance(game_object, Bagable):
                engine.print_terminal("Object is bagable")
            engine.print_terminal(game_object.get_name())

    def pick_up_objects(self):
        """ Pick up and put all bagable objects in front of the player in the player's bag.

        """
        engine = self.get_engine()
        #object_list = engine.get_objects_at(location_in_fron_of_player)
        #for(object in object_list):
        #	if(type(object) == Bagable):
        #		bag.insert(object)
        return

    def bag_items_string(self):
        #result = ""
        #for(item in bag)
        #	result += item.name + " : " + item.property + "\n"
        #return result
        return "coconut_one : weight: 5"

    def run_script(self):
        """ Runs the current script in the player_scripts folder in a seperate thread. Exposes the PyGuide API to the script to allow it to control this player. :)

        Everything in the API is bocking, however this doesn't impact regular gameplay as it's run in a seperate thread.
        The callback is run after the script has finished running.

        TODO: work out if the callback should know if the script failed or not.
        (Or if callback is even needed)

        Parameters
        ----------
        script_name : str
            The name of the script you wish to running, in the player_scripts folder in the root of the game.
        callback : function
            The callback function you wish to run after the script has finished runnning.
        """
        if not(self.__running_script): #only run script if one currently isn't running.
            engine = self.get_engine()
            self.__running_script = True
    
            #script_api is a python dictionary of python objects (variables, methods, class instances etc.)
            #available to the player. :)
            #eg. if there is an entry named "fart" whos entry is blob, then in the level script, any reference to fart
            #will be refering to what blob is known as here.
            #Here the list of game_objects is being looped through, and their names are being mapped to each instance :)
            script_api = {}

            # Provide all the movement functions to the player, but make them blocking.
            script_api["move_north"] = scriptrunner.make_blocking(self.move_north)
            script_api["move_east"] = scriptrunner.make_blocking(self.move_east)
            script_api["move_south"] = scriptrunner.make_blocking(self.move_south)
            script_api["move_west"] = scriptrunner.make_blocking(self.move_west)

            script_api["print_bag_items"] = lambda: user_print(self.bag_items_string())

            #the method to get the position of the player
            script_api["get_position"] = self.get_position

            script_api["test_display_bag"] = self.test_display_bag

            scriptrunner.start(script_api, engine.get_run_script(), self)
        return

    def halt_script(self):
        """ Halts the player script that is running.

        Works by sending the thread the script is running in an Exception, which the thread catches and appropriately handles and
        stops running.
        """
        if self.__running_script:
            thread_id = self.__thread_id #TODO: Make this process safer, look at temp.py and add appropriate guards around the next line to check for valid results etc.
            res = ctypes.pythonapi.PyThreadState_SetAsyncExc(ctypes.c_long(thread_id), ctypes.py_object(scriptrunner.HaltScriptException))

    def set_running_script_status(self, status):
        """ Set the script runnin status of the player, used by scriptrunner.py as a simple check to see if this player is already running as script.

        Simply prevents to scripts with player inputs from running simultaneously.
        """
        self.__running_script = status
        return

    def set_thread_id(self, thread_id):
        self.__thread_id = thread_id
        return

    def get_thread_id(self):
        return thread_id

    """ private:
    Put the private methods you wish to use here.
    """

    """ Override character move methods to prevent movement if script is running
    """
    def __input_move_north(self, callback = lambda: None):
        if (not self.__running_script) and (not self.is_moving()) and (not self.is_busy()): #Check that a script isn't running
            def callback_wrap():
                self.__trigger_walk_on() #call walk-on triggers on objects player walks on
                callback()
            if self.is_facing_north():
                self.move_north(callback_wrap)
            else:
                self.wait(0.07, lambda: self.face_north() if not self.is_moving() else lambda: None)
        return

    def __input_move_east(self, callback = lambda: None):
        if (not self.__running_script) and (not self.is_moving()) and (not self.is_busy()): #Check that a script isn't running
            def callback_wrap():
                self.__trigger_walk_on() #call walk-on triggers on objects player walks on
                callback()
            if self.is_facing_east():
                self.move_east(callback_wrap)
            else:
                self.wait(0.07, lambda: self.face_east() if not self.is_moving() else lambda: None)
        return

    def __input_move_south(self, callback = lambda: None):
        if (not self.__running_script) and (not self.is_moving()) and (not self.is_busy()): #Check that a script isn't running
            def callback_wrap():
                self.__trigger_walk_on() #call walk-on triggers on objects player walks on
                callback()
            if self.is_facing_south():
                self.move_south(callback_wrap)
            else:
                self.wait(0.07, lambda: self.face_south() if not self.is_moving() else lambda: None)
        return

    def __input_move_west(self, callback = lambda: None):
        if (not self.__running_script) and (not self.is_moving()) and (not self.is_busy()): #Check that a script isn't running
            def callback_wrap():
                self.__trigger_walk_on() #call walk-on triggers on objects player walks on
                callback()
            if self.is_facing_west():
                self.move_west(callback_wrap)
            else:
                self.wait(0.07, lambda: self.face_west() if not self.is_moving() else lambda: None)
        return

    def __trigger_action(self):
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

    """ This method takes the movement input of the player character and returns the appropriate
    function for moving them in the direction required
    face_x -- self.face_north/east/south/west() as appropriately required to get them to face in that direction
    """
    #def __handle_movement_input(self, is_facing_x, face_x, move_x):
    #	def handle_input:
    #		if(not(self.is_moving())):  #can't register input if the character is in the middle of moving
    #			if(is_facing_x()): #if facing in x direction, get them to move in that direction, else face in that direction first
    #				move_x()
    #			else:
    #				face_x()
    #	return handle_input




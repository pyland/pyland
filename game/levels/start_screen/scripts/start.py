""" A prototyped menu for the demo, the actual demo in the game will probably not work like this but it is really worth having one """

import sys
config = engine.get_config()
sys.path.insert(1, config['files']['script_running_location'])
from script_state_container import ScriptStateContainer

camera.focus()
engine.hide_bag()
engine.clear_scripter()
c1 = (210, 210, 210)
c2 = (190, 190, 190)
engine.set_ui_colours(c1, c2)
engine.play_music("heroic") #Menus have herioc music always
engine.show_py_scripter()
engine.set_py_tabs(1)
engine.enable_py_scripter()
engine.insert_to_scripter("#Message to all Pyland Citizens:\n#- Use the arrow keys to select menu options\n#- Press 'Enter' to select an option\n#- Use the following command to load a saved game:\nload_game(\"\")")

def load_game(save_name):
    save_data = engine.get_player_data(save_name)
    engine.set_player_name(save_name)
    engine.change_map(save_data["current_map"])

#Let the player run python scripts in the menu...
stc = ScriptStateContainer()
script_api = {
    "load_game" : load_game,
    "load_map" : engine.change_map
}
stc.set_script_name("Menu")
engine.register_input_callback(engine.INPUT_RUN, lambda: stc.run_script(script_api, engine)) ##TODO: could provide some customer easter egg api's in the menu? :) (cheatcodes?)
engine.register_input_callback(engine.INPUT_HALT, stc.halt_script)


#The actual menu implementation...
def set_current_state(next_state, callback = lambda: None):
    """ A simple callback that is run to set the current menu state after the camera has finished moving """
    global current_state
    current_state = next_state
    engine.add_event(callback)


class MenuState:
    """ A menu state is a way of defining what each input does when the player is on a particular option (state) in the menu """

    location = (0, 0)          #The state's location in the map
    up_desination = None       #The state that will be changed to when up is pressed
    down_destination = None    #The state that will be changed to when down is pressed
    select_destination = None  #The state that will be changed to when the state is selected

    def __init__(self, location):
        self.location = location
        #Default behaviour is that each input maintains the current state
        self.up_desination = self
        self.down_destination = self
        self.select_destination = self

    #The implementaion for changing state based on each input, these can be overridden as well to provide specific behavious (eg. change a setting)
    #By default, the "current state" will change, and the camera will move to the location of the given "next state" based on the input
    def move_bracket(self, next_pos, time = 0.2, callback = lambda: None):
        x,y = next_pos
        left.move_to((x-4,y), time)
        right.move_to((x+4,y), time)

        engine.add_event(callback)

    def select(self):
        if not left.is_moving() and not camera.is_moving():
            next_state = self.select_destination
            engine.run_callback_list_sequence([
                lambda callback: camera.move_to(next_state.location, time = 0, callback = callback),
                lambda callback: set_current_state(next_state, callback = callback),
                lambda callback: self.move_bracket(next_state.location, callback = callback),
            ])

    def navigate_up(self):
        if not left.is_moving():
            next_state = self.up_destination
            engine.run_callback_list_sequence([
                #lambda callback: camera.move_to(next_state.location, time = 0.2, callback = callback),
                lambda callback: set_current_state(next_state, callback = callback),
                lambda callback: self.move_bracket(next_state.location)
            ])


    def navigate_down(self):
        if not left.is_moving():
            next_state = self.down_destination
            engine.run_callback_list_sequence([
                #lambda callback: camera.move_to(next_state.location, time = 0.2, callback = callback),
                lambda callback: set_current_state(next_state, callback = callback),
                lambda callback: self.move_bracket(next_state.location)
            ])



#Add all the menu states
start_game = MenuState((17, 10))    #start the game....
options = MenuState((17, 8))        #move to the options menu
language = MenuState((34, 10))      #change the language from the options menu
options_back = MenuState((34, 8))   #the "back" option in the options menu
english = MenuState((51, 10))       #the option to change the language to english
pyrate = MenuState((51, 8))         #the option to change the language to pyrate
language_back = MenuState((51, 6))  #the "back" option in the language menu


#Load in game settings
settings = engine.get_settings()

#Setting up how all the menu states interact with each other
start_game.down_destination = options
start_game.select = lambda: engine.change_map("/world1/intro")

options.up_destination = start_game
options.select_destination = language

language.down_destination = options_back
language.select_destination = english

options_back.up_destination = language
options_back.select_destination = start_game

def save_language(language):
    settings["language"] = language
    engine.save_settings(settings)
    engine.print_terminal("saved :" + language)

english.down_destination = pyrate
english.select = lambda: save_language("english")

pyrate.up_destination = english
pyrate.down_destination = language_back
pyrate.select = lambda: save_language("pyrate")

language_back.up_destination = pyrate
language_back.select_destination = language

#The current state is start_game, best option to land the player on by default!
current_state = start_game
camera.move_to(current_state.location)

#some wrapper functions which are needed to expose the current state's methods globally
def navigate_up():
    current_state.navigate_up()

def navigate_down():
    current_state.navigate_down()

def select():
    current_state.select()

#registering the input callbacks...
engine.register_input_callback(engine.INPUT_UP, navigate_up)
engine.register_input_callback(engine.INPUT_DOWN, navigate_down)
engine.register_input_callback(engine.INPUT_ACTION, select) ##TODO, change this to a better key!

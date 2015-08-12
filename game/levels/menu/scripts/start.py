import sys
config = engine.get_config()
sys.path.insert(1, config['files']['components_location'])
from script_state_container import ScriptStateContainer

camera.focus()
c1 = (210, 210, 210)
c2 = (190, 190, 190)
engine.set_ui_colours(c1, c2)
engine.play_music("heroic")


stc = ScriptStateContainer()
script_api = {}
stc.set_script_name("Menu")

engine.register_input_callback(engine.INPUT_RUN, lambda: stc.run_script(script_api, engine)) ##TODO: could provide some customer easter egg api's in the menu? :)
engine.register_input_callback(engine.INPUT_HALT, stc.halt_script)

def set_current_state(next_state):
    global current_state
    current_state = next_state

class MenuState:
    location = (0, 0)
    up_desination = None
    down_destination = None
    select_destination = None

    def __init__(self, location):
        self.location = location
        self.up_desination = self
        self.down_destination = self
        self.select_destination = self

    def select(self):
        if not camera.is_moving():
            next_state = self.select_destination
            camera.move_to(next_state.location, time = 0.2, callback = lambda: set_current_state(next_state))

start_game = MenuState((17, 10))
options = MenuState((17, 8))
language = MenuState((33, 10))
options_back = MenuState((33, 8))

start_game.down_destination = options
start_game.select = lambda: engine.print_terminal("Start the game!!!")

options.up_destination = start_game
options.select_destination = language

language.down_destination = options_back

options_back.up_destination = language
options_back.select_destination = options



current_state = start_game

def navigate_up():
    global camera
    global engine
    #states
    global start_game
    global options

    global current_state
    if not camera.is_moving():
        next_state = current_state.up_destination
        camera.move_to(next_state.location, time = 0.2, callback = lambda: set_current_state(next_state))

def navigate_down():
    global camera
    global engine
    #states
    global start_game
    global options

    global current_state
    if not camera.is_moving():
        next_state = current_state.down_destination
        camera.move_to(next_state.location, time = 0.2, callback = lambda: set_current_state(next_state))

def select():
    current_state.select()

engine.register_input_callback(engine.INPUT_UP, navigate_up)
engine.register_input_callback(engine.INPUT_DOWN, navigate_down)

engine.register_input_callback(engine.INPUT_TALK, select) ##TODO, change this to a better key!

import random

player_one.focus()

engine.play_music("heroic")
engine.set_ui_colours((200,255,200),(215,255,215)) #TODO: save these colours in the config.
engine.set_py_tabs(2)

#overwrite the move_west and move_north functions for the player so that the player cannot move up the ledges

def move_west(callback = lambda: None):
    self.face_west()
    self.move_on_spot(callback)

def move_north(callback = lambda: None):
    self.face_north()
    self.move_on_spot(callback)

player_one.move_north = move_north
player_one.move_west = move_west


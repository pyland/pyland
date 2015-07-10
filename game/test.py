import operator
import os

import sys
sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/objects/game_object/.')
from game_object import GameObject

test_object_one = GameObject("test_object_one")

test_object_one.move_north(lambda: print("Testing callback"))

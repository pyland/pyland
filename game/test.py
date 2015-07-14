import operator
import os
import sys
sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/objects/characters/enemies/crocodile')
from crocodile import Crocodile


test_object_one = Crocodile("test_object_one")
test_object_one.follow_path("east, north, south, west", True)
print("woop")

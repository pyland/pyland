import operator
import os
import sys
sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/objects/characters/enemies/crocodile')
from crocodile import Crocodile


croc_one = Crocodile("croc_one")


croc_one.follow_path("east, north, south, west", True)
print("woop")

#call_back_test(lamda: print("hello"))

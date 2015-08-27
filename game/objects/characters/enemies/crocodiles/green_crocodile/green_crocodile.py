import sys
import os
import random

sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/..')
from crocodile import Crocodile

class GreenCrocodile(Crocodile):
    """ The crocodiles are a class of enemy """

    def initialise(self):
        super().initialise()
        self.oscillate = -1









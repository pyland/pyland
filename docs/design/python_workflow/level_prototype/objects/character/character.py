"""
In Python comments,
could define some standard which the C++ code can use to determine things about it handles 
the python code
"""

"""
eg. We could be able to write:
"""
#__ import_object GameObject
"""
The game code, at runtime, could recognise the "#__"
and replace it with:
"""
import sys
sys.path.insert(0, '../game_object')
from game_object import GameObject
"""
As the GameObject is in the base objects folder.
"""

import operator

""" This class is the parent of all characters in the game
It provides a lot of useful prebuilt functionality, mainly to do with movement animation.

Lets say you have an object called 'x' in objects/x, by default a sprite will be looked for in objects/x/x.png, if none is found,
the object will be invisible in game.

However, a character MUST have at least the following sprites in the specified (relative to objects/x) locations.

If the sprite location is changed, the following relative paths to that location must still be valid sprites.

main/north/1.png
main/north/2.png
main/north/3.png
main/north/4.png

main/east/1.png
main/east/2.png
main/east/3.png
main/east/4.png

main/south/1.png
main/south/2.png
main/south/3.png
main/south/4.png

main/west/1.png
main/west/2.png
main/west/3.png
main/west/4.png


Here the first folder represents the state of the character. (All characters MUST have a "main" state)
The second folder represents the direction they are facing.
The filename represents the frames of animation of the character when they are moving. (The first frame is implicitly assumed to be used when they are still)

This benifits of this class is that it provides very simple methods for movement + animation.
However, it doesn't check if the tiles that are being moved to are empty or not. TODO: Talk about maybe changing this?
""" 
class Character(GameObject):

	__state = "main"
	__moving = False
	__facing = "north"

	""" Move character north one tile. """
	def moveNorth(self):
		""" Pseudo code.
		Moving = True
		Start animation
		Start moving north
		Once finished moving = false"""
		return

	def moveSouth(self):
		return

	def moveEast(self):
		return

	def moveWest(self):
		return

	def faceNorth(self):
		return
	
	def faceEast(self):
		return

	def faceSouth(self):
		return

	def faceNorth(self):
		return

	def getFacing(self):
		return __facing

	def isFacing(self, direction):
		""" Pseudo code should check which direction character is facing and 
		see if it is the same. Furthermore it should check if a valid direction has been provided and provide a warning otherwise
		"""
		return True

	def changeState(self, state):
		if(isinstance(state, str)):
			self.__state = state
		return
	

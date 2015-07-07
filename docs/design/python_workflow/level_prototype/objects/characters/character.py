import operator
import os
"""
In Python comments,
could define some standard which the C++ code can use to determine things about it handles 
the python code
"""

"""
eg. We could be able to write:
"""
#__ import_object  game_object/GameObject
"""
The game code, at runtime, could recognise the "#__"
and replace it with:
"""
import sys
sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/../game_object')
from game_object import GameObject
"""
As the GameObject is in the base objects folder.
"""


""" This class is the parent of all characters in the game
It provides a lot of useful prebuilt functionality, mainly to do with movement animation.

Lets say you have an object called 'x' in objects/x, by default a sprite will be looked for in objects/x/sprites/main.png, if none is found,
the object will be invisible in game.

However, a character MUST have at least the following sprites in the specified (relative to objects/x/sprites) locations.

If the sprite location is changed, the following relative paths to that location must still be valid sprites.
main/north/1.png (state/direction_facing/animation_frame.png)
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

	def __init(self):
		super().__init__()
		self.set_sprite("main/north/1.png")

	""" Returns True if the character is moving,
	False otherwise.
	"""
	def moving(self):
		return self.__moving

	""" Move character in facing direction one tile. """
	def move(self):
		""" Pseudo code.
		moving = True
		Start animation
		Start moving in direction facing
		Once finished, moving = false"""
		return

	def face_north(self):
		return
	
	def face_east(self):
		return

	def face_south(self):
		return

	def face_north(self):
		return

	def get_facing(self):
		return __facing

	def before_frame_update(self):
		super().before_frame_update()
		self.__handle_movement_input()

	def is_facing(self, direction):
		""" Pseudo code should check which direction character is facing and 
		see if it is the same. Furthermore it should check if a valid direction has been provided and provide a warning otherwise
		"""
		return True

	def change_state(self, state):
		if(isinstance(state, str)):
			self.__state = state
		return
	

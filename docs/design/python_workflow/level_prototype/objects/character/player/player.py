"""
In Python comments,
could define some standard which the C++ code can use to determine things about it handles 
the python code
"""

"""
eg. We could be able to write:
"""
#__ import_object Character
"""
The game code, at runtime, could recognise the "#__"
and replace it with:
"""
import sys
sys.path.insert(0, '../../game_object/character')
from character import Character
"""
As the GameObject is in the base objects folder.
"""

import operator

""" A brief description of the class will go here.
The auto-generated comment produced by the script should also mention where to get a list of the api
and which built-in variables exist already.
"""
class Player(Character):
	
	""" constructor
	run when the object is created in-engine
	"""
	def __init__(self, initArray, name):
		super().__init__(initArray)
		#self.setSprite(), overwrite what could be grabbed from tiled
		print('I am alive!!!! I am ' + name) #Maybe have a debug option as well?
	
	""" game engine features (public)
	These are methods which the game engine will execute at the commented moments.
	This will all be autofilled by the creation script with super filled in to help
	prevent errors when it comes to this.
	"""

	""" This method is run every frame before the graphics are displayed.
	You can put code here you want to run before every frame.
	You MUST but super().beforeFrameUpdate() for this to work. Otherwise this may lead
	to unexpected behaviour.
	"""
	def beforeFrameUpdate(self):
		super().beforeFrameUpdate()
		self.__handleMovementInput()

	""" public:
	Put the regular public methods you wish to use here.
	"""

	""" private:
	Put the private methods you wish to use here.
	"""
	
	""" This method takes the movement input of the player character and appropriately
	handles it.
	"""
	def __handleMovementInput(self):
		if(!self.moving()):  #can't register input if the character is in the middle of moving
			(x, y, z) = self.getLocation() #NB z is the layer number of the object
			if(isInputPressed(CONST_UP)):
				if(!self.isFacing("north")):
					self.faceNorth()
				else:
					(x, y) = map(operator.add, (x, y),(0, 1))
					if(isLocationFree(x, y, z)):
						self.moveNorth()
			else if(isInputPressed(CONST_DOWN)):
				if(!self.isFacing("south")):
					self.faceSouth()
				else:
					(x, y) = map(operator.add, (x, y),(0, -1))
					if(isLocationFree(x, y, z)):
						self.moveSouth()
			else if(isInputPressed(CONST_RIGHT)):
				if(!self.isFacing("east")):
					self.faceEast()
				else:
					(x, y) = map(operator.add, (x, y),(1, 0))
					if(isLocationFree(x, y, z)):
						self.moveEast()
			else if(isInputPressed(CONST_LEFT)):
				if(!self.isFacing("west")):
					self.faceWest()
				else:
					(x, y) = map(operator.add, (x, y),(-1, 0))
					if(isLocationFree(x, y, z)):
						self.moveWest()

test = Player([], "Larry")
test.beforeFrameUpdate()

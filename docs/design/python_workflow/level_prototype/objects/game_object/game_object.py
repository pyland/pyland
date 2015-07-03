class GameObject: #An example of the base game object
	visible = True;
	solid = True;

	sprite = "same as this folder" #have it so that by default, if the inheriting file is in objects/x/y, the default sprite is sprites/x/y/y.png.
	#^^ should be manually changable though

	def __init__(self, initArray):
		print("I am teh basic.")

	def beforeFrameUpdate(self):
		print("Woo Ice Cream!!")

	""" Stub for built-in api """
	def getLocation(self):
		return (12, 7)

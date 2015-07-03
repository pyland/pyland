from test import *

class Coin(GameObject):
	
	def f(self, name):
		print('hello ' + name)

	#def collide(self, PlayerCharacter)

gameObject = GameObject()
coin = Coin()
coin.f('Tom')

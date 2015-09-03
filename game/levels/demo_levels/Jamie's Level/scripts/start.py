#Implement your level logic here!

player_one.focus()

player_one.set_character_name("PyPlayer")

engine.play_music("world_1_jungle")

def dig():
	pos = player_one.get_position()
	x,y = pos
	engine.print_terminal(pos)
	isdead = engine.get_tile_type(pos) == engine.TILE_TYPE_QUICKSAND
	if isdead:
		engine.print_terminal("You died!")
	else:
		total = 0
		for i in range(-1,2):
			for j in range(-1,2):
				if i==j==0: continue
				if engine.get_tile_type((x+i,y+j)) == engine.TILE_TYPE_QUICKSAND:
					total += 1
		engine.print_terminal("%s mines around!" % total)		
		return total



player_one.dig = dig

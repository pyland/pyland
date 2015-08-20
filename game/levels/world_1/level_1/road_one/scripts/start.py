player_one.focus()


village_sign.set_message("(west) rossumberg (north) bog (north) merchant")
bog_sign.set_message("(south) rossumberg (east) bog (north) merchant")
merchant_sign.set_message("(south) rossumberg (south) bog (north) merchant")

annoying_croc.face_west()

player_start_pos = player_one.get_position()
player_one.move_east(callback = lambda: heidi.move_to(player_start_pos, callback = lambda: heidi.follow(player_one)))

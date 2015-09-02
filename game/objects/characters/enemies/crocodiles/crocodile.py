import sys
import os
import random

sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/..')
from enemy import Enemy

class Crocodile(Enemy):
    """ The crocodiles are a class of enemy """

    def initialise(self):
        super().initialise()
        self.__check_swim_state()
        self.oscillate = 0

    def __check_swim_state(self, callback = lambda: None):
        """ This is used by the crocodiles to determine wether or not they are in water (and show the swimming sprite if they are).

        Parameters
        ----------
        callback : func, optional
            Places the callback onto the engine event-queue

        """

        engine = self.get_engine()
        x, y = self.get_position()
        if(engine.get_tile_type((x, y)) == engine.TILE_TYPE_WATER):
            self.change_state("swim")
        engine.add_event(callback)

    def move_north(self, callback = lambda: None):
        super().move_north(lambda: self.__check_swim_state(callback))
        engine = self.get_engine()
        x, y = self.get_position()
         #if the crocodile is about to move to a land position, show land sprite before movement begins (gives best results)
        if(engine.get_tile_type((x, y+1)) == engine.TILE_TYPE_STANDARD):
            self.wait(0.1, lambda: self.change_state("main")) #delay there so that crocs that go over water change sprite for a flash.
    
    def move_east(self, callback = lambda: None):
        super().move_east(lambda: self.__check_swim_state(callback))
        engine = self.get_engine()
        x, y = self.get_position()
        if(engine.get_tile_type((x+1, y)) == engine.TILE_TYPE_STANDARD):
            self.wait(0.1, lambda: self.change_state("main"))

    def move_south(self, callback = lambda: None):
        super().move_south(lambda: self.__check_swim_state(callback))
        engine = self.get_engine()
        x, y = self.get_position()
        if(engine.get_tile_type((x, y-1)) == engine.TILE_TYPE_STANDARD):
            self.wait(0.1, lambda: self.change_state("main"))

    def move_west(self, callback = lambda: None):
        super().move_west(lambda: self.__check_swim_state(callback))
        engine = self.get_engine()
        x, y = self.get_position()
        if(engine.get_tile_type((x-1, y)) == engine.TILE_TYPE_STANDARD):
            self.wait(0.1, lambda: self.change_state("main"))

    def player_action(self, player_object):
        """ This is the method that is run if the player presses the action key while facing a character.
        
        """
        pass


    def toggle(self):
        """ Toggles the crocodile and makes him face the opposite direction
        """
        if(self.is_facing_north()):
            return self.face_south()
        if(self.is_facing_south()):
            return self.face_north()
        if(self.is_facing_east()):
            return self.face_west()
        if(self.is_facing_west()):
            return self.face_east()

    def move_horizontal(self, player, times = -1):
        """ The crocodile moves horizontally (east/west direction).

        Everytime there is a collision, times is decremented. When time hits zero, the crocodile stops moving and holds in place. The toggle is needed to the crocodile faces the correct direction when stopping.
        Whilst moving the crocodile checks if there is a player adjacent to it, if so it eats the player and the player loses.

        Parameters
        ----------
        player : Player
            The player the crocodile is looking for
        times : int, optional
            Number of times the crocoilde can collide with the edge of a riverbank before stopping.
        """

        x,y = self.get_position()
        player_x, player_y = player.get_position()
        engine = self.get_engine()

        if x == player_x:
            if player_y == y+1:
                return self.face_north(lambda: self.lose(player))
            elif player_y == y-1:
                return self.face_south(lambda: self.lose(player))
        elif y == player_y:
            if player_x == x+1:
                return self.face_east(lambda: self.lose(player))
            elif player_x == x-1:
                return self.face_west(lambda: self.lose(player))


        if times != 0:
            if(self.is_facing_west()):
                if not engine.get_tile_type((x-1,y)) == engine.TILE_TYPE_WATER or engine.is_solid((x-1,y)):
                        return self.face_east(lambda: self.move_horizontal(player, times-1))
                else:
                    return self.move_west(lambda: self.move_horizontal(player, times))
            elif(self.is_facing_east()):
                if not engine.get_tile_type((x+1,y)) == engine.TILE_TYPE_WATER or engine.is_solid((x+1,y)):
                        return self.face_west(lambda: self.move_horizontal(player, times-1))
                else:
                        return self.move_east(lambda: self.move_horizontal(player, times))
        else:
            return self.toggle()

    def move_vertical(self, player, times = -1): 
        """ The crocodile moves vertically (north/south direction).

        Everytime there is a collision, times is decremented. When time hits zero, the crocodile stops moving and holds in place. The toggle is needed to the crocodile faces the correct direction when stopping.
        Whilst moving the crocodile checks if there is a player adjacent to it, if so it eats the player and the player loses.

        Parameters
        ----------
        player : Player
            The player the crocodile is looking for
        times : int, optional
            Number of times the crocoilde can collide with the edge of a riverbank before stopping.
        """
        x,y = self.get_position()
        player_x, player_y = player.get_position()
        engine = self.get_engine()

        if x == player_x:
            if player_y == y+1:
                return self.face_north(lambda: self.lose(player))
            elif player_y == y-1:
                return self.face_south(lambda: self.lose(player))
        elif y == player_y:
            if player_x == x+1:
                return self.face_east(lambda: self.lose(player))
            elif player_x == x-1:
                return self.face_west(lambda: self.lose(player))


        if times != 0:
            x,y = self.get_position()
            if(self.is_facing_north()):
                if not engine.get_tile_type((x,y+1)) == engine.TILE_TYPE_WATER or engine.is_solid((x,y+1)):
                    return self.face_south(lambda: self.move_vertical(player, times-1))
                else:
                    return self.move_north(lambda: self.move_vertical(player, times))
            elif(self.is_facing_south()):
                if not engine.get_tile_type((x,y-1)) == engine.TILE_TYPE_WATER or engine.is_solid((x,y-1)):
                        return self.face_north(lambda: self.move_vertical(player, times-1))
                else:
                    return self.move_south(lambda: self.move_vertical(player, times))

        if times == 0:
            return self.toggle()

    def yelled_at(self, player):
        """ This is run whenever the player yells at the crocodile

        If the crocodile is still and the player is facing the crocodile and yells, the crocodile will start to oscillate

        Parameters
        ----------
        player : Player
            The player that gets caught by the crocodile
        """
        player_x, player_y = player.get_position()
        self_x, self_y = self.get_position()
        if not self.is_moving():
            if player_y == self_y:
                if player_x < self_x and player.is_facing_east():
                    self.move_west(lambda: self.move_horizontal(player, self.oscillate))
                elif player_x > self_x and player.is_facing_west():
                    self.move_east(lambda: self.move_horizontal(player, self.oscillate))
                return
            if player_x == self_x:
                if player_y < self_y and player.is_facing_north():
                    self.move_south(lambda: self.move_vertical(player, self.oscillate))
                elif player_y > self_y and player.is_facing_south():
                    self.move_north(lambda: self.move_vertical(player, self.oscillate))
                return
            
    def lose(self, player):
        """ This is run whenever the player gets caught by a crocodiles

        Parameters
        ----------
        player : Player
            The player that gets caught by the crocodile
        """
        engine = self.get_engine()
        engine.run_callback_list_sequence([
            lambda callback: player.set_busy(True, callback = callback),
            lambda callback: engine.show_dialogue("Crocodile: I've got you!", callback = callback),
        ], player.kill)
    









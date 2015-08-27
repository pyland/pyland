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

    """ The Crocodile follows the path given, a string a comma-seperated directions, eg. "north, east, east, south, west, north"
    If repeat is set to True, the most recent direction completed will be added to the end of the string so that whole thing becomes a cycle
    """
    def follow_path(self, path, repeat = False):
        if(path.strip() == ""): #if path is empty terminate
            return
        
        engine = self.get_engine()
        x, y = self.get_position()

        comma_location = path.find(",") # Find the first comma in the path
        if(comma_location == -1):  # No commas in the path! On last word!
            comma_location = len(path)

        old_path = path #store the old_path
        
        instruction = path[ 0 : comma_location].strip() #get instruction and remove whitespace
        path = path[comma_location + 1: ].strip() #remove the instruction from the path itself
        if(repeat):
            path = path + ", " + instruction #add instruction back to the path, at the end if to be repeated
        if(instruction == "north"):
            if engine.is_solid((x, y+1)): #if position isn't walkable, then wait
                self.face_north()
                return self.wait(0.3, lambda: self.follow_path(old_path, repeat))
            else:
                return self.move_north(lambda: self.follow_path(path, repeat))
        elif(instruction == "east"):
            if engine.is_solid((x+1, y)): #if the position you are trying to move to is taken, wait
                self.face_east()
                return self.wait(0.3, lambda: self.follow_path(old_path, repeat))
            else:
                return self.move_east(lambda: self.follow_path(path, repeat))
            return
        elif(instruction == "south"):
            if engine.is_solid((x, y-1)):
                self.face_south()
                return self.wait(0.3, lambda: self.follow_path(old_path, repeat))
            else:
                return self.move_south(lambda: self.follow_path(path, repeat))
        elif(instruction == "west"):
            if engine.is_solid((x-1, y)):
                self.face_west()
                return self.wait(0.3, lambda: self.follow_path(old_path, repeat))
            else:
                return self.move_west(lambda: self.follow_path(path, repeat))
        else:
            pass #TODO: handle invalid path!!!!!
            print(instruction)
            print(path)

        return


    def rand_explore(self):
        """ The Crocodile follows a random path forever!
        """
        rand = random.randint(1, 5)
        if rand==1:
            return self.move_north(self.rand_explore)
        elif rand==2:
            return self.move_south(self.rand_explore)
        elif rand==3:
            return self.move_east(self.rand_explore)
        elif rand==4:
            return self.move_west(self.rand_explore)
        elif rand==5:
            return self.wait(0.3, self.rand_explore)



    def __check_swim_state(self, callback = lambda: None):
        """ This is used by the crocodiles to determine wether or not they are in water (and show the swimming sprite if they are).

        It is called as a callback once the crocodile has finished moving as that gives the smoothest change-over.
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
        if(self.is_facing_north()):
            return self.face_south()
        if(self.is_facing_south()):
            return self.face_north()
        if(self.is_facing_east()):
            return self.face_west()
        if(self.is_facing_west()):
            return self.face_east()

    def move_horizontal(self, player, times = -1):
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
        engine = self.get_engine()
        engine.run_callback_list_sequence([
            lambda callback: player.set_busy(True, callback = callback),
            lambda callback: engine.show_dialogue("Crocodile: I've got you!", callback = callback),
        ], player.kill)
    









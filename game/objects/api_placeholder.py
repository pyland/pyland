import multiprocessing
import time

class API:

    object_properties = {}
    
    """ Given the folder-location of a set of sprites, the object has those sprites displayed as it's sprites """
    def set_sprite(self, game_object, sprite_location):
        print('API: set sprite location of "{}" to "{}"'.format(game_object.get_name(), sprite_location))
        #self.object_properties[game_object.get_name()]["sprite_location"] = sprite_location # a placeholder way of holding the information
        return

    def get_sprite(self, game_object):
        return ""

    """ Set the visisbilty of the object's sprites, there must be valid sprites in the folder location for this to be set to true!!! """
    def set_visible(self, game_object, visible):
        print('API: set object visibilty of "{}" to "{}"'.format(game_object.get_name(), visible))
        return

    """ returns true if the object's sprite is set to visible, else otherwise """
    def is_visible(self, game_object):
        return

    """ Solid objects cannot be walked through """
    def set_solid(self, game_object, solid):
        print('API: set solidity of "{}" to "{}"'.format(game_object.get_name(), solid))
        return

    """ Returns true if the object is solid, false otherwise """
    def is_solid(self, game_object):
        return

    """ Moves the object north, returns immediately but object takes time to actually move """
    def move_north(self, game_object, callback):
        p = multiprocessing.Process(target = lambda: self.__move("north", callback)) #Make it so that calls to this queue or ignored if object is moving???
        p.start()
        return

    """ Moves the object east, returns immediately but object takes time to actually move """
    def move_east(self, game_object, callback):
        p = multiprocessing.Process(target = lambda: self.__move("east", callback))
        p.start()
        return

    """ Moves the object south, returns immediately but object takes time to actually move """
    def move_south(self, game_object, callback):
        p = multiprocessing.Process(target = lambda: self.__move("south", callback))
        p.start()
        return

    """ Moves the object west, returns immediately but object takes time to actually move """
    def move_west(self, game_object, callback):
        p = multiprocessing.Process(target = lambda: self.__move("west", callback))
        p.start()
        return

    def __move(self, direction, callback):
        print("Moving {}...".format(direction))
        time.sleep(2) #Make a game engine clock version!!!!
        print("Moving {} complete.".format(direction))
        callback()
        return

    """ Returns true if the object is moving, false otherwise """
    def moving(self, game_object):
        return True
        

    """ Immediately sets the position of the object to be that given by the 3-tuple """
    def set_position(self, game_object, position):
        return

    """ Returns a 3-tuple of the objects position """
    def get_position(self, game_object):
        return (0, 0, 0)
    
    """ Starts animating the object based on the files in the sprite folder """
    def start_animating(self, game_object):
        return

    """ Pauses the animation on the current frame """
    def pause_animating(self, game_object):
        return

    """ Sets the animation frame of the object to frame_number modulo the number of frames it has """
    def set_frame(self, game_object, frame_number):
        return

    """ destroys an object and removes it from the game """
    def destroy(self, game_object):
        return

    """ Changes the area which the player is in and puts them in the place specified """
    def area_change(self, area, position):
        return
        
        

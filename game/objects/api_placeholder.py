import multiprocessing
import time
import queue

class ObjectProperties:
    data = {}
    
    def set_property(self, object_name, property_name, property_value):
        if not(object_name in data):
            data[object_name] = {}
        data[object_name][property_name] = property_value


class EventQueue:
        queue = queue.Queue()
        started = False
        event_process = None

        def add_event(self, event):
            self.queue.put(event)

        def run(self):
            if(not(self.started)):
                self.started = True
                self.event_process = multiprocessing.Process(target = self.__r).start()
            return

        def __r(self):
            i = 0
            while(True):
                if not self.queue.empty():
                    self.queue.get()()

class API:

    object_properties = ObjectProperties()
    event_queue = EventQueue()
    
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
        self.__move("north", callback)
        return

    """ Moves the object east, returns immediately but object takes time to actually move """
    def move_east(self, game_object, callback):
        self.__move("east", callback)
        return

    """ Moves the object south, returns immediately but object takes time to actually move """
    def move_south(self, game_object, callback):
        self.__move("south", callback)
        return

    """ Moves the object west, returns immediately but object takes time to actually move """
    def move_west(self, game_object, callback):
        self.__move("west", callback)
        return

    def __move(self, direction, callback): #add movement and callback to object's event queue!
        def move() :
            print("Moving {}...".format(direction))
            #time.sleep(2) #Make a game engine clock version!!!!
            print("Moving {} complete.".format(direction))
        self.event_queue.add_event(move)		
        self.event_queue.add_event(callback)
        self.event_queue.run() #For some reason it has to be called after these, but apart from that it is all fine? very strange
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
        
        

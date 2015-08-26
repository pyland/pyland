import sys
import os
from queue import PriorityQueue
import vertex as Vertex

sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/../../../characters')
#sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/../../../game_object')
#from game_object import GameObject
from character import Character #TODO: it should import from here, but some kind of definitions with the movement functions are screwing around with follow_path, causing the game to freeze.

import random

""" A brief description of the class will go here.
The auto-generated comment produced by the script should also mention where to get a list of the api
and which built-in variables exist already.
"""
class Monkey(Character):
    def initialise(self):
        super().initialise()

    def player_action(self, player_object):
        """ This is the method that is run if the player presses the action key while facing a character.
        
        TODO: move this to character.py and make it print an explenation on how to override this by default.
        """
        pass

    def find_escape_path(self, callback = lambda: None):
        engine = self.get_engine()
        self_x, self_y = self.get_position()
        x_dim = 19
        y_dim = 19

        vertex = [] 
        #Accesses y then x

        for y in range(y_dim):
            new_list = []
            for x in range(x_dim):
                new_list.append(Vertex.Vertex(x,y))
            vertex.append(new_list)


        for y in range(y_dim):
            for x in range(x_dim):
                current = vertex[y][x]
                #NORTH
                if y + 1 < y_dim:
                    if not engine.is_solid((x, y+1)):
                        current.neighbors.append(vertex[y+1][x])
                #SOUTH
                if y - 1 >= 0:
                    if not engine.is_solid((x, y-1)):
                        current.neighbors.append(vertex[y-1][x])
                #EAST
                if x + 1 < x_dim:
                    if not engine.is_solid((x+1, y)):
                        current.neighbors.append(vertex[y][x+1])
                #WEST
                if x - 1 < x_dim:
                    if not engine.is_solid((x-1, y)):
                        current.neighbors.append(vertex[y][x-1])

        pq = []
        start = vertex[self_y][self_x]
        start.distance = 0
        pq.append(start)
        end = start
        start.parent = start

        while(len(pq) != 0):
            current = pq[0]
            for v in pq:
                if v < current:
                    current = v
            pq.remove(current)
            
            if current.x == 0 or current.x == x_dim - 1 or current.y == 0 or current.y == y_dim -1:
                end = current
                break

            for neighbor in current.neighbors:
                if current.distance + 1 < neighbor.distance and not neighbor.visited:
                    neighbor.distance = current.distance + 1
                    neighbor.parent = current
                    if not neighbor in pq:
                        pq.append(neighbor)

            current.visited = True

        if end != start:
            while end.parent != start:
                end = end.parent
            if start.x + 1 == end.x:
                self.move_east(lambda: self.find_escape_path())
            elif start.x - 1 == end.x:
                self.move_west(lambda: self.find_escape_path())
            elif start.y + 1 == end.y:
                self.move_north(lambda: self.find_escape_path())
            elif start.y - 1 == end.y:
                self.move_south(lambda: self.find_escape_path())

        engine.add_event(callback)
            

    


        
    






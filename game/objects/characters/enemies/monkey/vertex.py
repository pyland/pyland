class Vertex:
    def __init__(self, x, y):
        self.x = x
        self.y = y
        self.neighbors = []
        self.parent = None 
        self.distance = float("inf")
        self.visited = False
    def __lt__(self, other):
        return (self.distance - other.distance) < 0
    def __str__(self):
        return "(" + str(self.x) + "," + str(self.y) + ")"


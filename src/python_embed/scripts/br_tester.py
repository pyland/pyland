import bootstrapper_reborne

class DummyEntity:

    name = ""

    def __init__(self, name):
        self.name = name

    def get_location(self):
        return "characters/enemies/crocodile"
    def get_name(self):
        return self.name

class DummyEngine:
    def get_level_location(self):
        return "test_world/test_level/test_one"
    def print_debug(self, string):
        print(string)
        return

test_one = DummyEntity("boulder_one")
test_two = DummyEntity("boulder_two")
test_three = DummyEntity("boulder_three")
test_four = DummyEntity("croc_one")
test_five = DummyEntity("player_one")
engine = DummyEngine()

entities = [test_one, test_two, test_three, test_four, test_five]

bootstrapper_reborne.start(entities, BaseException, BaseException, BaseException, False, engine)



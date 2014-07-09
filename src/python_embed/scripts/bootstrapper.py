import time

print("Started bootstrapper")

def start(entity):
    print("Started with entity", entity)

    try:
    	run(entity)
    except BaseException as e:
    	print(e)
    	raise

def run(entity):
    """Purely for testing"""

    print("Running with entity", entity)
	
    for _ in range(500000):
        time.sleep(0.0001)

        import random
        entity.move(random.randint(-4, 4), random.randint(-4, 4))
        print("Continuing with entity", entity)

    print("Finishing with entity", entity)

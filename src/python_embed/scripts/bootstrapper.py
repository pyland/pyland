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
    
    for _ in range(5):
        time.sleep(0.1)

        import random
        entity.move(random.randint(-1, 1), random.randint(-1, 1))
        print("Continuing with entity", entity)

    print("Finishing with entity", entity)

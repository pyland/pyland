import time

print("Started bootstrapper")

def start(object):
    print("Started with object", object)

    try:
    	run()
    except BaseException as e:
    	print(e)
    	raise

def run():
    print("Running with object", object)
	
    for _ in range(100):
        time.sleep(0.1)

        print("Continuing with object", object)

    print("Finishing with object", object)

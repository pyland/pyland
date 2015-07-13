import multiprocessing
import time

def sleeper():
    print("sleep...")
    time.sleep(5)
    print("awake!!!")

def test():
    print("woop")
    p = multiprocessing.Process(target = sleeper)
    p.start()
    print("testing")

test()

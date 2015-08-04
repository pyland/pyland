import code
import sys
import io
import contextlib

""" TODO: CLEAN THIS UP, THIS UP A LOT (do the same in bootsrapper.py), this is a really really really
weird, and as far as I can see, unnesecary way of defining a class. But what do I know :P
TODO: Make it so that the scoped_intepreter in bootstrapper.py is pulled from here.

Creates and defines a class called ScopedIntepreter which is used to run the player-code.
"""
class ScopedInterpreter(code.InteractiveInterpreter):

    output_reader = None

    """ Creates a scoped intepreter which can be used to run some code which is provided as the argument to runcode()
    imbued_locals -- A dictionary from strings to objects, each object will be available to the code being run in the interpreter under the name given by their dictionary key
    output_reader -- A function which takes a string as an argument, any output from the scoped code will be passed to here: TODO: work out if this is what it actually does!
    """
    def __init__(self, imbued_locals, output_reader):
        super().__init__(imbued_locals)
        self.output_reader = output_reader

    """ TODO: work out what this does, was in bootstrapper.py so assumed to be needed here!"""
    def write(self, data):
        pass
        #self.output_reader(data, True)

    def runcode(self, code):
        old_stdout = sys.stdout
        sys.stdout = io.TextIOWrapper(io.BytesIO(), sys.stdout.encoding)

        #"""TODO: work out how to catch exceptions safely and handle them with python,
        #then we can query the database to print output and errors messages cleanly to the user!"""
        #TODO: work out what the hell contextlib.closing does, don't forget to change bootstrapper.py based on that!
        try:
            with contextlib.closing(sys.stdout):
                pass
                # Emulate super().runcode(code), but don't catch
                # RESTART, STOP or KILL because those should
                # propogate to allow thread control.
                #
                # The code here is a trivial modification of the
                # source of the implementaion we're overloading.
    
                try:
                    exec(code, self.locals)
                except (SystemExit): #, RESTART, STOP, KILL, TODO: work out what system_exit actually is!
                    raise
                except:
                    self.showtraceback() # TODO: work out how this works, comment it in bootstrapper.py as well
                # Read
                sys.stdout.seek(0) #TODO: fix why this is was unseekable
                output = sys.stdout.read()
    
        finally:
            sys.stdout = old_stdout
    
        if output:
            """ This if statement is called whenever user calls "print(data)" inside their code,
            TODO: make it so that it prints the output to the player console! However, this version
            only prints to the console once the script has finished as opposed to printing in real time?
            """
            #self.output_reader(output, False)



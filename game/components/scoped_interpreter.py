import code
import sys
import io
import contextlib
import traceback

class ScopedInterpreter(code.InteractiveInterpreter):
    """ Creates and defines a class called ScopedIntepreter which is used to run the player-code """

    error_output = print #This variable determines where errors are output

    """ Creates a scoped intepreter which can be used to run some code which is provided as the argument to runcode()
    imbued_locals -- A dictionary from strings to objects, each object will be available to the code being run in the interpreter under the name given by their dictionary key
    output_reader -- A function which takes a string as an argument, any output from the scoped code will be passed to here: TODO: work out if this is what it actually does!
    """
    def __init__(self, imbued_locals, error_output = print):
        super().__init__(imbued_locals)
        self.error_output = error_output

    def runcode(self, code, HaltScriptException):
        #"""TODO: work out how to catch exceptions safely and handle them with python,
        #then we can query the database to print output and errors messages cleanly to the user!"""
        #TODO: work out what the hell contextlib.closing does, don't forget to change bootstrapper.py based on that!
        with contextlib.closing(sys.stdout):
            # Emulate super().runcode(code), but don't catch
            # RESTART, STOP or KILL because those should
            # propogate to allow thread control.
            #
            # The code here is a trivial modification of the
            # source of the implementaion we're overloading.
            try:
                exec(code, self.locals)
            except HaltScriptException as He:
                raise He
            except Exception as e:
                #Print out any errors caught in the code to the game terminal
                self.error_output(traceback.format_exc()) 


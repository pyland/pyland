import code
import sys
import io
import contextlib
import traceback

class ScopedInterpreter(code.InteractiveInterpreter):
    """ Creates and defines a class called ScopedIntepreter which is used to run the player-code

    By creating an instance of this class, and using it to run any code the player writes. The functions, objects and values
    the player has access to can be tightly controlled. The script_api is a dictionary from names to the values the player
    has access to.
    """

    def __init__(self, script_api, error_output = print):
        """ Creates a scoped intepreter which can be used to run some code which is provided as the argument to runcode()

        Parameters
        ----------
        script_api : dict from string to object
            A dictionary from strings to objects, each object will be available to the code being run in the interpreter under the name given by their dictionary key
        error_output : string -> void
            A function which takes a string as it's argument. All error messages comming from the scripts run in the scoped intepreter will be passed to this method
        """
        super().__init__(script_api)
        self.error_output = error_output

    def runcode(self, code, HaltScriptException):
        """ Run the code that is passed as an argument to ScopedInterpreter.

        Parameters
        ----------
        code : str
            A string of the code you wish to run
        HaltScriptException : Exception
            An instance of the exception class you wish to use to halt the executing script asynchronously
        """

        #print("Py: scoped interpreter runcode")

        with contextlib.closing(sys.stdout):
            # Emulate super().runcode(code), but don't catch
            # RESTART, STOP or KILL because those should
            # propogate to allow thread control.
            #
            # The code here is a trivial modification of the
            # source of the implementaion we're overloading.
            try:
                #print("Py: executing code")
                exec(code, self.locals)
            except HaltScriptException as He:
                #print("Py: halt script execption")
                raise He
            except Exception as e:
                #Print out any errors caught in the code to error_output
                #print("Py: output error")
                self.error_output(traceback.format_exc())


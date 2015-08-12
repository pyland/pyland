
import ctypes #for sending exceptions to python threads!!!
import scriptrunner

class ScriptStateContainer:
    __script_name = "script_state_container"
    __thread_id = 0
    __running_script = False

    def set_running_script_status(self, status):
        """ Set the script runnin status of the player, used by scriptrunner.py as a simple check to see if this player is already running as script.

        Simply prevents to scripts with player inputs from running simultaneously.
        """
        self.__running_script = status
        return

    def is_running_script(self):
        return self.__running_script

    def get_script_name(self):
        return self.__script_name

    def set_script_name(self, script_name):
        self.__script_name = script_name

    def set_thread_id(self, thread_id):
        self.__thread_id = thread_id

    def get_thread_id(self):
        return self.__thread_id

    def halt_script(self):
        """ Halts the player script that is running.

        Works by sending the thread the script is running in an Exception, which the thread catches and appropriately handles and
        stops running.
        """
        if self.is_running_script():
            thread_id = self.get_thread_id()
            res = ctypes.pythonapi.PyThreadState_SetAsyncExc(ctypes.c_long(thread_id), ctypes.py_object(scriptrunner.HaltScriptException))

    def run_script(self, script_api, engine):
        """ Runs the current script in the player_scripts folder in a seperate thread. Exposes the script_api to the script that is run.

        Parameters
        ----------
        script_api : dict
            A dictionary from object names to their instances #TODO: explain this a bit more clearly
        """
        if not(self.is_running_script()): #only run script if one currently isn't running.
            self.set_running_script_status(True)
            scriptrunner.start(script_api, engine.get_run_script(), self, engine)
        return

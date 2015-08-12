


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

    def get_script_name(self):
        return self.__script_name

    def set_script_name(self, script_name):
        self.__script_name = script_name

    def set_thread_id(self, thread_id):
        self.__thread_id = thread_id

    def get_thread_id(self):
        return self.__thread_id

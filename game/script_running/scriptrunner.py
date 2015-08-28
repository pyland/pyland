import code
import sys
import io
import os
import contextlib
import time
import threading

from scoped_interpreter import ScopedInterpreter

""" This file contains all the implementation details of how player scripts are interpreted and run. """

class HaltScriptException(Exception):
    pass

def start(script_api, script_name, script_state_container, engine, parse_error = True, callback = lambda: None):
    """ This function runs the script provided in the argument in a seperate thread.

    The script has access to a set of API's defined in script_api that allow
    it to control the player object provided.

    the callback is run after the script has finished running.

    Parameters
    ----------
    script_state_container : ScriptStateContainer
        This is the instance of the script_state_container which is interacted with using the script.
    script_name : str
        The name of the script that you wish to run. The game looks in the script folder for it.
    """

    """ How printing is handled """

    printed_flag = [False]  #This flag used to determine if the player has printed something, because if nothing has been printed there is no point in inserting the "----" in the terminal.
    def user_print(text = ""):
        """ A simple method to print text to the game console for the user, overrides the python default print method """
        printed_flag[0] = True
        engine.print_terminal(text, False) #autoconvert print to strings (do not need to convert within the game)

    #Gets the position of the nth line number from the error message
    #Returns the cut down string that it was working with, and the position of the start and the end of the line number in that string
    def parse_line_number_index(string, n):
        parse_stage = 0

        start_char = 0
        end_char = 0

        #remove text from the start to the first 'line' of string
        for cur_line_remove in range(n-1):
            if ("line" in string):
                string = string[string.index("line")+4:len(string)]

        for char in range(string.index("line"),len(string)):
            if parse_stage == 0:
                if string[char] == " ":
                    start_char = char+1
                    parse_stage = 1
            elif parse_stage == 1:
                if ((string[char] == " ") or (string[char] == ",")):
                    end_char = char
                    parse_stage = 2
                    break

        return (start_char,end_char,string)


    #Gets the nth line number from the error message
    def parse_line_number(string, n):
        position = parse_line_number_index(string, n)
        #Get the line number string from the string returned by parse_line_number_index
        return position[2][position[0]:position[1]]

    #Parse any Python error output to be user friendly
    def parse_output(error_message):
        parsed_error_message = ""

        engine.set_error(True)

        #Add more elif statements for the error messages we want to parse to give more useful messages
        if ("NameError" in error_message):
            command = ""
            line = ""

            parse_stage = 0

            start_char = 0
            end_char = 0
            for char in range(error_message.index("NameError"),len(error_message)):
                if parse_stage == 0:
                    if error_message[char] == "'":
                        start_char = char+1
                        parse_stage = 1
                elif parse_stage == 1:
                    if error_message[char] == "'":
                        end_char = char
                        parse_stage = 2
                        break

            command = error_message[start_char:end_char]

            parse_stage = 0

            start_char = 0
            end_char = 0

            #Get the second line number from the error message
            line = parse_line_number(error_message,2)

            parsed_error_message = ("PyRunner Error: '"+ command +"' in line "+ line +" is not understood")
        elif ("SyntaxError" in error_message) and ("EOL" in error_message):
            #Get the section of the error after the second line number has been given
            #(this section shows an arrow to the part of the line with the issue)
            error_section_string = parse_line_number_index(error_message, 2)[2]
            error_start_index = parse_line_number_index(error_message, 2)[1]

            start_char = 0
            end_char = 0

            error_pointer = error_section_string[error_start_index:error_section_string.index("SyntaxError")]

            #Get the second line number from the error message
            line = parse_line_number(error_message,2)

            parsed_error_message = ("PyRunner Error: There is no end quotation mark for the string\n in line "+ line + "\n"+error_pointer)
        elif ("SyntaxError" in error_message) and ("EOF" in error_message):
            #Get the section of the error after the second line number has been given
            #(this section shows an arrow to the part of the line with the issue)
            error_section_string = parse_line_number_index(error_message, 2)[2]
            error_start_index = parse_line_number_index(error_message, 2)[1]

            start_char = 0
            end_char = 0

            error_pointer = error_section_string[error_start_index:error_section_string.index("SyntaxError")]

            #Get the second line number from the error message
            line = parse_line_number(error_message,2)

            parsed_error_message = ("PyRunner Error: There is no end bracket in line "+ line + "\n"+error_pointer)
        ##This only works if a print statement was used, fails if print is part of a variable name or string
#        elif ("SyntaxError" in error_message) and ("print" in error_message):
#            #Get the section of the error after the second line number has been given
#            #(this section shows an arrow to the part of the line with the issue)
#            error_section_string = parse_line_number_index(error_message, 2)[2]
#            error_start_index = parse_line_number_index(error_message, 2)[1]
#
#            start_char = 0
#            end_char = 0
#
#            error_pointer = error_section_string[error_start_index:error_section_string.index("SyntaxError")]
#
#            #Get the second line number from the error message
#            line = parse_line_number(error_message,2)
#
#            parsed_error_message = ("PyRunner Error: The 'print' command in line "+ line +" should use brackets and only join strings together using '+'\n"+error_pointer+"\nMake sure your print statements look like any of the following:\nprint('Hello World') \nprint('Hello '+name)\nprint('I am ' + str(age) + ' years old')")
        elif ("SyntaxError" in error_message):
            #Get the section of the error after the second line number has been given
            #(this section shows an arrow to the part of the line with the issue)
            error_section_string = parse_line_number_index(error_message, 2)[2]
            error_start_index = parse_line_number_index(error_message, 2)[1]

            start_char = 0
            end_char = 0

            error_pointer = error_section_string[error_start_index:error_section_string.index("SyntaxError")]

            #Get the second line number from the error message
            line = parse_line_number(error_message,2)

            parsed_error_message = ("PyRunner Error: Invalid Python code in line "+ line + "\n"+error_pointer)

        else:
            #Default error message, in case the error has not been handled above
            #Print all of the text from the second line number

            error_section_string = parse_line_number_index(error_message, 2)[2]
            error_start_index = parse_line_number_index(error_message, 2)[0]

            start_char = 0
            end_char = 0

            #include the text 'line' in the default output
            error_message_shortened = error_section_string[error_start_index:len(error_section_string)]

            engine.print_terminal("PyRunner Error: Line "+error_message_shortened, True)
            engine.print_terminal("---" + script_state_container.get_script_name() + "'s script has terminated early---", False)
            return

        engine.print_terminal(parsed_error_message, True)
        engine.print_terminal("---" + script_state_container.get_script_name() + "'s script has terminated early---", False)

        printed_flag[0] = False


    #Replace print statement in player script so that all their output goes to the terminal. (unless it has already been overidden)
    if not "print" in script_api:
        script_api["print"] = user_print

    #Instantiate the scoped intepreter

    if (parse_error):
        scoped_interpreter = ScopedInterpreter(script_api, lambda error_output: parse_output(error_output)) #create an instance of it
    else:
        scoped_interpreter = ScopedInterpreter(script_api, lambda error_output: engine.print_terminal(error_output, True)) #create an instance of it
    script_filename = engine.get_config()['files']['player_scripts'] + "/"+str(script_name)+".py" #grab the absolute location of the script TODO: implement this path stuff in a config (ini) file!!!!!

    #open and read the script
    with open(script_filename, encoding="utf8") as script_file:
                script = script_file.read()

    def thread_target(callback):
        """ This is the method that is run in the seperate thread.

        It runs the script requested first and then runs the callback.
        the callback is therefore run in the seperate thread.
        """

        try:
            engine.set_error(False)
            scoped_interpreter.runcode(script, HaltScriptException) #Run the script
        except HaltScriptException: #If an exception is sent to halt the script, catch it and act appropriately
            engine.print_terminal("Halted Script", True)
            printed_flag[0] = True
        finally:
            if printed_flag[0]:
                engine.print_terminal("---" + name + "'s script has ended---", False)
            script_state_container.set_running_script_status(False)
            engine.set_finished()
            engine.add_event(callback)

    thread = threading.Thread(target = lambda: thread_target(callback))

    engine.print_debug(script)

    thread.start()
    script_state_container.set_thread_id(thread.ident) #Save the player's thread id so that scripts can be halted
    return

def make_blocking(async_function):
    """ Takes an asynchronous function as an argument and returns a version of it that is blocking.

    Works by using callbacks to work out when the function has finished.
    Therefore the function must be a function which takes a callback as an argument which runs when event
    that the function initiates has finished.
    """
    def blocking_function():
        """ This is the blocking version of the async_function that is provided as and argument. """
        lock = threading.Lock()
        lock.acquire()
        result = async_function(callback = lock.release) #run the async_function with the callback provided above as its argument
        lock.acquire() # Try to a acquire a lock until it is released. It isn't released until the callback releases it.
        lock.release() # release the lock, it isn't needed anymore
        return result

    return blocking_function


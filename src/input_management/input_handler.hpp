#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <unordered_map>
#include <list>
#include <functional>

///
/// The Input Handler class, this is a singleton that is used to unify input behaviours and allows
/// Python Callbacks to be registered against certain inputs. When events are registered against particular
/// inputs, they are put on an event list registered against that input. When that input is registered, all
/// those events put onto the event list for the next frame to be run then.
/// TODO: work out how to handle holding inputs down, double cliking etc???
/// TODO: work out underlying datastructure
class InputHandler {
    typedef std::list<std::function<void()>> function_list; //Makes a standard list of void functions less of a mouthfull :P

public:
    ///
    /// Getter for the main global event manager.
    /// @return a reference to the global event manager
    ///
    static InputHandler* get_instance();

    static int get_input_keyboard_binder(char input_key);

    static const int INPUT_UP = 10;
    static const int INPUT_RIGHT = 11;
    static const int INPUT_DOWN = 12;
    static const int INPUT_LEFT = 13;
    static const int INPUT_RETURN = 14;
    static const int INPUT_ONE = 1;
    static const int INPUT_TWO = 2;
    static const int INPUT_THREE = 3;
    static const int INPUT_FOUR = 4;
    static const int INPUT_FIVE = 5;
    static const int INPUT_SIX = 6;
    static const int INPUT_SEVEN = 7;
    static const int INPUT_EIGHT = 8;
    static const int INPUT_NINE = 9;

    static const int INPUT_RUN = 111;
    static const int INPUT_HALT = 112;
    static const int INPUT_TOGGLE_SPEED = 777;

    static int constexpr INPUT_KEYBOARD(char raw_key); //TODO: Work out if this is needed.

    ///
    /// Puts all of the functions registered against an input on the event queue.
    /// This is called when an inpu is pressed with the relevant associated key.
    ///
    /// @param input_key
    /// 	The identifier of the input who's list you wish to run.
    ///
    void run_list(int input_key);

    ///
    /// Flushes the function list associated with an input.
    ///
    /// @param input_key
    /// 	The identifier of the input who's list you wish to flush.
    ///
    void flush_list(int input_key);

    ///
    /// Deletes all the lists with input callbacks associated against them.
    ///	Used to flush everything.
    ///
    void flush_all();

    ///
    /// Register an input callback against an input.
    ///
    /// @param input_key
    /// 	The identifier of the input that you wish to register against.
    ///
    void register_input_callback(int input_key, std::function<void()> input_callback); //TODO:return handler which can be used to remove from list?

private:

    InputHandler();
    ~InputHandler();

    InputHandler(InputHandler const&) = delete;
    void operator=(InputHandler const&) = delete;

    std::unordered_map<int, function_list*> event_map; //A map from integers to list of functions pointers

    ///
    /// Grabs the function list associated with a given input.
    /// If no such list exists, one is created at runtime,
    ///
    /// @param input_key
    /// 	The identifier of the input who's list you wish to grab.
    ///
    /// @return
    /// 	A pointer to the relevant function list.
    ///
    function_list *grab_list(int input_key);
};

#endif

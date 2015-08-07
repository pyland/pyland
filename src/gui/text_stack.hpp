#ifndef TEXT_STACK_H
#define TEXT_STACK_H

#include <list>
#include <string>

///
/// A class to hold a cache of text messages
///
class TextStack {
private:
    ///
    /// stack list of current and passed messages
    ///
    std::list<std::string> message_stack;

    ///
    /// iterator to locate the users position in the message history
    ///
    std::list<std::string>::iterator stack_iterator;

    void update_flags();

    bool _can_forward;
    bool _can_backward;
	bool _reached_end;

public:
    TextStack();

	bool can_forward() {return _can_forward;}
    bool can_backward() {return _can_backward;}
	bool reached_end() {return _reached_end;}
    ///
    /// add new appends a new message string to the TextStack
    /// @param new_message
    ///
    void add_new(std::string new_message);

    ///
    /// moves back in history and return new string to display
    /// @return
    ///     the new string to display in the text box if the movement
    ///     was invalid, then returns the same string
    ///
    std::string backward();

    ///
    /// moves forward in history and return new string to display
    /// @return
    ///     the new string to display in the text box, if the movement
    ///     was invalid, then returns the same string
    ///
    std::string forward();

    ///
    /// Clear the stack
    ///
    void clear();

    ///
    ///returns the current string
    ///
    std::string present();
};

#endif

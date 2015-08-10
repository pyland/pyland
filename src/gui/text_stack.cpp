#include <iterator>
#include <list>
#include <string>
#include <glog/logging.h>
#include "text_stack.hpp"

TextStack::TextStack():
    _can_forward(false),
    _can_backward(false)
{
    stack_iterator = message_stack.end();
}

void TextStack::add_new(std::string new_message) {
    message_stack.push_back(new_message);

    if(stack_iterator == message_stack.end()){
        stack_iterator = message_stack.begin();
    }

    update_flags();
}

std::string TextStack::backward() {
    // Empty stack
    if (message_stack.empty()) {
        _reached_end = true;
        return ("");
    }

    // Don't go off the en... start.
    if (stack_iterator == message_stack.begin()) {
        _reached_end = true;
        return ("");
    }

    //else it's fine
    --stack_iterator;

    update_flags();
    return *stack_iterator;
}

std::string TextStack::forward() {
    // Empty stack
    if (message_stack.empty()) {
        _reached_end = true;
        return ("");
    }

    ++stack_iterator;

    // Don't go off the end.
    if(stack_iterator == message_stack.end())
    {
        //restore it to the one before end
        --stack_iterator;
        _reached_end = true;
        return ("");
    }

    update_flags();
    return *stack_iterator;
}

std::string TextStack::present() {

    if(message_stack.empty()){
        _reached_end = true;
        return("");
    }

    else return *stack_iterator;
}

void TextStack::update_flags() {

    if(stack_iterator == message_stack.end()){
        _can_backward = false;
        _can_forward = false;
    }

    if (stack_iterator == message_stack.begin()) {
        _can_backward = false;
    } else {
        _can_backward = true;
    }

    if (stack_iterator == std::prev(message_stack.end())  ) {
        _can_forward = false;
    } else {
        _can_forward = true;
    }
}

void TextStack::clear() {

    message_stack.clear();
    stack_iterator = message_stack.end();

    update_flags();
}

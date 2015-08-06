#include <iterator>
#include <list>
#include <string>

#include "text_stack.hpp"

TextStack::TextStack():
    can_forward(false),
    can_backward(false) {
    stack_iterator = std::end(message_stack);
}

void TextStack::add_new(std::string new_message) {
    message_stack.push_back(new_message);
    stack_iterator = std::prev(std::end(message_stack));

    update_flags();
}

std::string TextStack::backward() {
    // Empty stack
    if (stack_iterator == std::end(message_stack)) {
        return ("");
    }

    // Don't go off the en... start.
    if (stack_iterator != std::begin(message_stack)) {
        --stack_iterator;
    }

    update_flags();
    return *stack_iterator;
}

std::string TextStack::forward() {
    // Empty stack
    if (stack_iterator == std::end(message_stack)) {
        return "";
    }

    // Don't go off the end.
    if (stack_iterator != std::prev(std::end(message_stack))) {
        ++stack_iterator;
    }

    update_flags();
    return *stack_iterator;
}

void TextStack::update_flags() {
    if (stack_iterator == std::begin(message_stack)) {
        can_backward = false;
    } else {
        can_backward = true;
    }

    if (stack_iterator == std::prev(std::end(message_stack))) {
        can_forward = false;
    } else {
        can_forward = true;
    }
}

void TextStack::clear() {
    message_stack.clear();
    stack_iterator = std::end(message_stack);
}

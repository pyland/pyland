#include <iterator>
#include <list>
#include <string>

#include "notification_stack.hpp"

Notification::Notification(): 
    can_forward(false),
    can_backward(false) {
    stack_iterator = std::end(message_stack);
}

void Notification::add_new(std::string new_notifcation) {
    message_stack.push_back(new_notifcation);
    stack_iterator = std::prev(std::end(message_stack));

    update_flags();
}

std::string Notification::backward() {
    // Empty stack
    if (stack_iterator == std::end(message_stack)) {
        return "";
    }

    // Don't go off the en... start.
    if (stack_iterator != std::begin(message_stack)) {
        --stack_iterator;
    }

    update_flags();
    return *stack_iterator;
}

std::string Notification::forward() {
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

void Notification::update_flags() {
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

void Notification::clear() {
    message_stack.clear();
    stack_iterator = std::end(message_stack);
}

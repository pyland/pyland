#include <iterator>
#include <list>
#include <notification_stack.hpp>
#include <string>

#include "notification_stack.hpp"

Notification::Notification() {
    stack_iterator = std::end(message_stack);
}

void Notification::add_new(std::string new_notifcation) {
    message_stack.push_back(new_notifcation);
    stack_iterator = std::prev(std::end(message_stack));
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

    return *stack_iterator;
}


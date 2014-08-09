#include <notification.hpp>
#include <glog/logging.h>

Notification::Notification() {
    stack = std::list<std::string>();
    stack_iterator = stack.end();
    // to correct iterator as end() is after the last element
    stack_iterator--;
}

void Notification::add_new(std::string new_notifcation) {
    stack.push_back(new_notifcation);
    stack_iterator = stack.end();
    // to correct iterator as end() is after the last element
    stack_iterator--;
}

std::string Notification::backward() {
    if (stack_iterator != stack.begin()) {
        VLOG(2) << "displaying previous text";
        return *--stack_iterator;
    } else {
        VLOG(2) << "no pass notification to display";
        return *stack_iterator;
    }
}

std::string Notification::forward() {
    // to correct iterator as end() is after the last element
    if (stack_iterator != --(stack.end()) ) {
        VLOG(2) << "displaying next text";
        return *++stack_iterator;
    } else {
        VLOG(2) << "at current notification, no more to display";
        return *stack_iterator;
    }
    
}


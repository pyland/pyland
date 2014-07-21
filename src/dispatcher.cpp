#include <functional>
#include <map>

Dispatcher::CallbackID Dispatcher::register(std::function<bool (Arguments...)> callback) {
    functions[maxid++] = callback;
    return maxid;
}

void Dispatcher::unregister(Dispatcher::CallbackID callback) {
    if (!functions.erase(callback)) {
        throw new std::runtime_error("Nonexistent callback");
    }
}

void Dispatcher::trigger() {
    // Do increments inline
    for (auto it = functions.cbegin(); it != functions.cend(); ) {
        if ((*it)()) {
            // Warning:
            // Must be post-increment, and increment
            // must be done before erasing.
            functions.erase(it++);
        }
        else {
            ++it;
        }
    }
}

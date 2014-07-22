#include <functional>
#include <map>

template <typename... Arguments>
Dispatcher::CallbackID Dispatcher::register(std::function<bool (Arguments...)> callback) {
    functions[maxid++] = callback;
    return maxid;
}

void Dispatcher::unregister(Dispatcher::CallbackID callback) {
    if (!functions.erase(callback)) {
        throw new std::runtime_error("Nonexistent callback");
    }
}

template <typename... Arguments>
void Dispatcher::trigger(Arguments... arguments) {
    // Do increments inline
    for (auto it = functions.cbegin(); it != functions.cend(); ) {
        if ((*it)(arguments...)) {
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

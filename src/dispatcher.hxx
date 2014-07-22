#include <functional>
#include <map>

template <typename... Arguments>
typename Dispatcher<Arguments...>::CallbackID Dispatcher<Arguments...>::register_callback(std::function<bool (Arguments...)> callback) {
    functions[maxid++] = callback;
    return maxid;
}

template <typename... Arguments>
void Dispatcher<Arguments...>::unregister(Dispatcher<Arguments...>::CallbackID callback) {
    if (!functions.erase(callback)) {
        throw new std::runtime_error("Nonexistent callback");
    }
}

template <typename... Arguments>
void Dispatcher<Arguments...>::trigger(Arguments... arguments) {
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

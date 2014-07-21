#include <functional>
#include <map>

template <typename... Arguments>
class Dispatcher {
    using CallbackID = uint64;

    public:
        CallbackID register(std::function<bool (Arguments...)>);
        void unregister(CallbackID callback);
        void trigger(Arguments... arguments);

    private:
        uint64 maxid = 0;
        std::map<CallbackID, std::function<bool (Arguments...)>> functions;
};

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

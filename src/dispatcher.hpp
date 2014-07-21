#include <functional>
#include <map>

template <typename... Arguments>
class Dispatcher {
    public:
        using CallbackID = uint64;
        CallbackID register(std::function<bool (Arguments...)>);
        void unregister(CallbackID callback);
        void trigger(Arguments... arguments);

    private:
        uint64 maxid = 0;
        std::map<CallbackID, std::function<bool (Arguments...)>> functions;
};

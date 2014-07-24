#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <functional>
#include <map>

template <typename... Arguments>
class Dispatcher {
    public:
        using CallbackID = uint64_t;
        CallbackID register_callback(std::function<bool (Arguments...)> callback);
        void unregister(CallbackID callback);
        void trigger(Arguments... arguments);

    private:
        uint64_t maxid = 0;
        std::map<CallbackID, std::function<bool (Arguments...)>> functions;
};

#include "dispatcher.hxx"

#endif

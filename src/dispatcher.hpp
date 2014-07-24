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

template <typename... Arguments>
class PositionDispatcher {
    public:
        using CallbackID = uint64_t;
        CallbackID register_callback(<std::pair<int, int>, std::function<bool (Arguments...)> callback);
        void unregister(CallbackID callback);
        void trigger(Arguments... arguments);

    private:
        uint64_t maxid = 0;
        std::vector < std::vector < std::map<CallbackID, std::function<bool (Arguments...)>> > > callback_map
};



#include "dispatcher.hxx"

#endif




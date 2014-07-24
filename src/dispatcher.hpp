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

class PositionDispatcher {
    public:
        using CallbackID = uint64_t;
        CallbackID register_callback( std::pair<int, int> tile, std::function<bool (int)> callback);
        void unregister( std::pair<int, int> tile, CallbackID callback);
        void trigger(std::pair<int, int> tile, int id);

    private:
        uint64_t maxid = 0;
        std::vector < std::vector < std::map<CallbackID, std::function<bool (int)>> > > callback_map;
};



#include "dispatcher.hxx"

#endif




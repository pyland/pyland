#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <functional>
#include <glm/vec2.hpp>
#include <map>
#include <utility>

template <typename... Arguments>
class Dispatcher {
    public:
        using CallbackID = uint64_t;
        CallbackID register_callback(std::function<bool (Arguments...)> callback);
        bool unregister(CallbackID callback);
        void trigger(Arguments... arguments);

    private:
        uint64_t maxid = 0;
        std::map<CallbackID, std::function<bool (Arguments...)>> functions;
};

template <typename... Arguments>
class PositionDispatcher {
    public:
        PositionDispatcher(glm::ivec2 location);
        using CallbackTileID = uint64_t;
        using CallbackID = std::pair<glm::ivec2, CallbackTileID>;
        CallbackID register_callback(glm::ivec2 tile, std::function<bool (Arguments...)> callback);
        bool unregister(CallbackID callback);
        void trigger(glm::ivec2 tile, Arguments... arguments);

    private:
        uint64_t maxid = 0;

        std::vector<std::vector<
            std::map<CallbackTileID, std::function<bool (Arguments...)>>
        >> callback_map;
};



#include "dispatcher.hxx"

#endif

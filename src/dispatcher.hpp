#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <functional>
#include <glm/vec2.hpp>
#include <map>
#include <stdint.h>
#include <utility>

template <typename... Arguments>

///
/// Dispatcher is used various places as a pub-sub directory.
/// Trigger calls the registered lambdas to be evaluated with the arguments specified.
///
class Dispatcher {
    public:
        using CallbackID = uint64_t;

        ///
        /// register_callback will subscribe the callback given to the dispatcher
        ///
        /// @param callback
        ///     lambda that takes the arguments specified in the template and
        ///     return a bool, specifying if this lambda is to be de registered.
        ///         true - continue calling this lambda
        ///         false - de register this lambda, same as calling unregister()
        ///
        /// @return
        ///     The ID allocated to the callback, can only be used to unregister
        ////
        CallbackID register_callback(std::function<bool (Arguments...)> callback);

        ///
        /// Remove the lambda from the dispatcher
        ///
        /// @param  callback
        ///     id associated with the callback which is being removed
        ///
        /// @return
        ///     if the callback was removed (casted from erase)
        ///
        bool unregister(CallbackID callback);

        ///
        /// call the registered callbacks
        ///
        void trigger(Arguments... arguments);

    private:
        uint64_t maxid = 0;
        std::map<CallbackID, std::function<bool (Arguments...)>> functions;
};

template <typename... Arguments>
///
/// A specialisation of Dispatcher to store a coordinate with each lambda.
/// @see Dispatcher
///
class PositionDispatcher {
    public:
        PositionDispatcher(glm::ivec2 size);
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

template <typename... Arguments>
typename Dispatcher<Arguments...>::CallbackID
Dispatcher<Arguments...>::register_callback(std::function<bool (Arguments...)> callback) {
    functions[++maxid] = callback;
    return maxid;
}

template <typename... Arguments>
bool Dispatcher<Arguments...>::unregister(Dispatcher<Arguments...>::CallbackID callback) {
    // erase by value returns number erased, which then gets coerced to a bool
    return functions.erase(callback);
}

template <typename... Arguments>
void Dispatcher<Arguments...>::trigger(Arguments... arguments) {
    // Do increments inline
    for (auto it = functions.cbegin(); it != functions.cend(); ) {
        if (!(*it).second(arguments...)) {
            // erase by iterator returns the next valid iterator
            it = functions.erase(it);
        }
        else {
            ++it;
        }
    }
}

template <typename... Arguments>
PositionDispatcher<Arguments...>::PositionDispatcher (glm::ivec2 size):
    callback_map(
        size.x,
        std::vector<std::map<CallbackTileID, std::function<bool (Arguments...)>>>(
            size.y,
            std::map<CallbackTileID, std::function<bool (Arguments...)>>()
        )
    )
{}

template <typename... Arguments>
typename PositionDispatcher<Arguments...>::CallbackID
PositionDispatcher<Arguments...>::register_callback(glm::ivec2 tile, std::function<bool (Arguments...)> callback) {
    callback_map[tile.x][tile.y][++maxid] = callback;
    return std::make_pair(tile, maxid);
}

template <typename... Arguments>
bool PositionDispatcher<Arguments...>::unregister(PositionDispatcher<Arguments...>::CallbackID callback) {
    return callback_map.at(callback.first.x).at(callback.first.y).erase(callback.second);
}

template <typename... Arguments>
void PositionDispatcher<Arguments...>::trigger(glm::ivec2 tile, Arguments... arguments) {
    // Do increments inline
    for (auto it = callback_map.at(tile.x).at(tile.y).cbegin(); it != callback_map.at(tile.x).at(tile.y).cend(); ) {
        if (!(*it).second(arguments...)) {
            it = callback_map[tile.x][tile.y].erase(it);
        }
        else {
            ++it;
        }
    }
}

#endif

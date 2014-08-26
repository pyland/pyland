#include <functional>
#include <glm/vec2.hpp>
#include <map>
#include <utility>

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

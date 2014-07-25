#include <functional>
#include <map>

template <typename... Arguments>
typename Dispatcher<Arguments...>::CallbackID

Dispatcher<Arguments...>::register_callback(std::function<bool (Arguments...)> callback) {
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
        if (!(*it)(arguments...)) {
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

template <typename... Arguments>
PositionDispatcher<Arguments...>::PositionDispatcher (Vec2D location):
    callback_map(
        location.x,
        std::vector<std::map<CallbackID, std::function<bool (Arguments...)>>>(
            location.y,
            std::map<CallbackID, std::function<bool (Arguments...)>>()
        )
    )
{}
 
template <typename... Arguments>
typename PositionDispatcher<Arguments...>::CallbackID PositionDispatcher<Arguments...>::register_callback(
        Vec2D tile, std::function<bool (Arguments...)> callback) {
    callback_map[tile.x][tile.y][maxid++] = callback;
    return maxid;
}

template <typename... Arguments>
void PositionDispatcher<Arguments...>::unregister( Vec2D tile, PositionDispatcher<Arguments...>::CallbackID callback) {
    if (!callback_map[tile.x][tile.y].erase(callback)) {
        throw new std::runtime_error("Nonexistent callback");
    }
}

template <typename... Arguments>
void PositionDispatcher<Arguments...>::trigger(Vec2D tile, Arguments... arguments) {
    // Do increments inline
    for (auto it = callback_map.at(tile.x).at(tile.y).cbegin(); it != callback_map.at(tile.x).at(tile.y).cend(); ) {
        if (!(*it).second(arguments...)) {
            // Warning:
            // Must be post-increment, and increment
            // must be done before erasing.
            callback_map[tile.x][tile.y].erase(it++);
        }
        else {
            ++it;
        }
    }
}
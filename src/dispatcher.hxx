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

typename PositionDispatcher::CallbackID PositionDispatcher::register_callback( std::pair<int, int> tile, std::function<bool (int)> callback) {
    callback_map[tile.first][tile.second][maxid++] = callback;
    return maxid;
}


void PositionDispatcher::unregister( std::pair<int, int> tile, PositionDispatcher::CallbackID callback) {
    if (!callback_map[tile.first][tile.second].erase(callback)) {
        throw new std::runtime_error("Nonexistent callback");
    }
}


void PositionDispatcher::trigger(std::pair<int, int> tile, int id) {
    // Do increments inline
    for (auto it = callback_map[tile.first][tile.second].cbegin(); it != callback_map[tile.first][tile.second].cend(); ) {
        if ((*it).second(id)) {
            // Warning:
            // Must be post-increment, and increment
            // must be done before erasing.
            callback_map[tile.first][tile.second].erase(it++);
        }
        else {
            ++it;
        }
    }
}
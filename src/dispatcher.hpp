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



#include "dispatcher.hxx"

#endif

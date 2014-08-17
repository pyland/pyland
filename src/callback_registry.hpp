#ifndef CALLBACK_REGISTRY_H
#define CALLBACK_REGISTRY_H

#include <glog/logging.h>
#include <ostream>
#include <set>

#include "callback.hpp"


///
/// Registry for callbacks.
///
template <typename Ret, typename... Args>
class CallbackRegistry {
private:
    ///
    /// All callbacks associated to the registry.
    ///
    std::set<Callback<Ret, Args...>> callbacks;

public:
    CallbackRegistry();
    ~CallbackRegistry();
    ///
    /// Register a callback.
    ///
    /// Does not register a callback more than once.
    ///
    void register_callback(const Callback<Ret, Args...> callback);
    ///
    /// Unregister a callback.
    ///
    void unregister_callback(const Callback<Ret, Args...> callback);
    ///
    /// Unregister a callback, given a pointer to one.
    ///
    void unregister_callback(const Callback<Ret, Args...>* callback);
    ///
    /// Unregister a callback, without notifying it.
    ///
    void unregister_callback_no_notify(const Callback<Ret, Args...> callback);
    ///
    /// Call all registered callbacks.
    ///
    /// @param args The arguments to pass to the callbacks.
    ///
    void broadcast(Args... args);
};


#include "callback_registry.hxx"

#endif

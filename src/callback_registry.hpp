#ifndef CALLBACK_REGISTRY_H
#define CALLBACK_REGISTRY_H

#include <set>



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
    ///
    /// Register a callback.
    ///
    /// Does not register a callback more than once.
    ///
    void register_callback(Callback<Ret, Args...> callback);
    ///
    /// Unregister a callback.
    ///
    void unregister_callback(Callback<Ret, Args...> callback);
    ///
    /// Unregister a callback, without notifying it.
    ///
    void unregister_callback_no_notify(Callback<Ret, Args...> callback);
    ///
    /// Call all registered callbacks.
    ///
    /// @param args The arguments to pass to the callbacks.
    ///
    void broadcast(Args... args);
};



#include "callback.hpp"

#include "callback_registry.hxx"

#endif

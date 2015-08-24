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

template <typename Ret, typename... Args>
CallbackRegistry<Ret, Args...>::CallbackRegistry():
    callbacks() {
}

template <typename Ret, typename... Args>
CallbackRegistry<Ret, Args...>::~CallbackRegistry() {
    // Save from the headache of iterator mutation.
    std::set<Callback<Ret, Args...>> callbacks_safe = callbacks;
    for (const Callback<Ret, Args...>& callback : callbacks_safe) {
        callback.remove_registry(this);
    }
    callbacks.clear();
}

template <typename Ret, typename... Args>
void CallbackRegistry<Ret, Args...>::register_callback(const Callback<Ret, Args...> callback) {
    callbacks.insert(callback);
    callback.add_registry(this);
}

template <typename Ret, typename... Args>
void CallbackRegistry<Ret, Args...>::unregister_callback(const Callback<Ret, Args...> callback) {
    VLOG(2) << "Removing callback " << callback.uid << " from registry " << this;
    callbacks.erase(callback);
    callback.remove_registry(this);
}

template <typename Ret, typename... Args>
void CallbackRegistry<Ret, Args...>::unregister_callback(const Callback<Ret, Args...>* callback) {
    VLOG(2) << "Removing callback " << callback->uid << " from registry " << this;
    callbacks.erase(*callback);
    callback->remove_registry(this);
}

template <typename Ret, typename... Args>
void CallbackRegistry<Ret, Args...>::unregister_callback_no_notify(const Callback<Ret, Args...> callback) {
    callbacks.erase(callback);
}

template <typename Ret, typename... Args>
void CallbackRegistry<Ret, Args...>::broadcast(Args... args) {
    // Save from the headache of iterator mutation.
    std::set<Callback<Ret, Args...>> callbacks_safe = callbacks;
    for (const Callback<Ret, Args...>& callback : callbacks_safe) {
        callback(args...);
    }
}

#endif

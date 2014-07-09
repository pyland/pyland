#include <set>
#include <utility>
#include <memory>
#include <iostream>

#ifdef DEBUG
#define CALLBACK_REGISTRY_DEBUG
#endif



template <typename Ret, typename... Args>
void CallbackRegistry<Ret, Args...>::register_callback(Callback<Ret, Args...> callback) {
    callbacks.insert(callback);
    callback.add_registry(this);
}


template <typename Ret, typename... Args>
void CallbackRegistry<Ret, Args...>::unregister_callback(Callback<Ret, Args...> callback) {
#ifdef CALLBACK_REGISTRY_DEBUG
    std::cerr << "Removing callback " << callback.uid << " from registry " << this << std::endl;
#endif
    callbacks.erase(callback);
    callback.remove_registry(this);
}


template <typename Ret, typename... Args>
void CallbackRegistry<Ret, Args...>::unregister_callback(Callback<Ret, Args...>* callback) {
#ifdef CALLBACK_REGISTRY_DEBUG
    std::cerr << "Removing callback " << callback->uid << " from registry " << this << std::endl;
#endif
    callbacks.erase(*callback);
    callback->remove_registry(this);
}


template <typename Ret, typename... Args>
void CallbackRegistry<Ret, Args...>::unregister_callback_no_notify(Callback<Ret, Args...> callback) {
    callbacks.erase(callback);
}


template <typename Ret, typename... Args>
void CallbackRegistry<Ret, Args...>::broadcast(Args... args) {
    // Save from the head ache of iterator mutation.
    std::set<Callback<Ret, Args...>> callbacks_safe = callbacks;
    for (const Callback<Ret, Args...>& callback : callbacks_safe) {
        callback(args...);
    }
}

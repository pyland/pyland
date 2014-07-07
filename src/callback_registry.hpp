#ifndef CALLBACK_REGISTRY_H
#define CALLBACK_REGISTRY_H

#include <set>
#include <function>



///
/// Registry for callbacks.
///
template <typename Ret, typename... Args>
class CallbackRegistry<typename Ret, typename... Args> {
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
    void register_callback(Callback<Ret, Args> callback) {
        callbacks.insert(callback);
        callback->add_registry(this);
    }
    ///
    /// Unregister a callback.
    ///
    void unregister_callback(Callback<Ret, Args> callback) {
        callbacks.erase(callback);
        callback->remove_registry(this);
    }
    ///
    /// Unregister a callback, without notifying it.
    ///
    void unregister_callback_no_notify(Callback<Ret, Args> callback) {
        callbacks.erase(callback);
    }
    ///
    /// Call all registered callbacks.
    ///
    /// @param args The arguments to pass to the callbacks.
    ///
    void broadcast(Args... args) {
        // Save from the head ache of iterator mutation.
        auto callbacks_safe = callbacks;
        for (auto& callback : callbacks_safe) {
            callback(args...);
        }
    }
}


#endif

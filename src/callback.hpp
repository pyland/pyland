#ifndef CALLBACK_H
#define CALLBACK_H

#include <set>
#include <function>

///
/// Allows registering a function with a callback registry.
///
template <typename Ret, typename... Args>
class Callback {
protected:
    friend class CallbackRegistry<Ret, Args...>;
    
    ///
    /// The callback function.
    ///
    std::function<Ret, Args...> func;
    ///
    /// A set of all registries the callback is registered to.
    ///
    std::shared_ptr<std::set<CallbackRegistry<Ret, Args...>>> registries;
    
    ///
    /// Notify about addition to a registry.
    ///
    virtual void add_registry(CallbackRegistry<Ret, Args...> registry) {
        registries->insert(registry);
    }
    
    ///
    /// Notify about removal from a registry.
    ///
    virtual void remove_registry(CallbackRegistry<Ret, Args...> registry) {
        registries->erase(registry);
    }

    ///
    /// Completely remove the callback from all associated registries.
    ///
    void unregister_everywhere() {
        auto registries_safe = registries;
        for (auto registry : registries_safe) {
            registry.unregister_callback(this);
        }
    }
public:
    Callback(Function func) {
        registries = std::make_shared<CallbackRegistry<Ret, Args...>>();
    }
    ~Callback() {
        // Deleting registries is not our job! It is handled by
        // shared_ptr.
    }
    
    ///
    /// Calls the callback function.
    ///
    Ret operator()(Args... args) {
        return func(args...);
    }
}
    
#endif

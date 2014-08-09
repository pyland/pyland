#include <functional>
#include <future>
#include <memory>

#include "event_manager.hpp"
#include "lifeline.hpp"
#include "locks.hpp"

template <typename T>
GilSafeFuture<T>::GilSafeFuture():
    return_value_promise(std::make_shared<std::promise<T>>()),
    return_value_lifeline([] () {})
    {}

template <typename T>
GilSafeFuture<T>::GilSafeFuture(std::shared_ptr<std::promise<T>> promise):
    return_value_promise(promise),
    return_value_lifeline([] () {})
    {}

template <>
GilSafeFuture<void>::GilSafeFuture(std::shared_ptr<std::promise<void>> promise);

template <typename T>
template <typename E>
GilSafeFuture<T>::GilSafeFuture(std::shared_ptr<std::promise<T>> promise,
                                typename std::enable_if<!std::is_void<E>::value, E>::type default_value):
    return_value_promise(promise),
    return_value_lifeline([promise, default_value] () { promise->set_value(default_value); })
    {}

template <typename T>
template <typename E, typename>
void GilSafeFuture<T>::set() {
    return_value_promise->set_value();
    return_value_lifeline.disable();
}

template <typename T>
template <typename E>
void GilSafeFuture<T>::set(typename std::enable_if<!std::is_void<E>::value, E>::type value) {
    return_value_promise->set_value(value);
    return_value_lifeline.disable();
}

template <typename T>
T GilSafeFuture<T>::execute(std::function<void (GilSafeFuture<T>)> callback) {
    auto return_value_promise = std::make_shared<std::promise<T>>();
    auto return_value_future = return_value_promise->get_future();

    {
        GilSafeFuture<T> gil_safe_return_value(return_value_promise);
        EventManager::get_instance().add_event(std::bind(callback, gil_safe_return_value));
    }

    {
        lock::ThreadGILRelease unlock_thread;
        return return_value_future.get();
    }
}

template <typename T>
template <typename E>
T GilSafeFuture<T>::execute(std::function<void (GilSafeFuture<T>)> callback,
                            typename std::enable_if<!std::is_void<E>::value, E>::type default_value) {
    auto return_value_promise = std::make_shared<std::promise<T>>();
    auto return_value_future = return_value_promise->get_future();

    {
        GilSafeFuture<T> gil_safe_return_value(return_value_promise, default_value);
        EventManager::get_instance().add_event(std::bind(callback, gil_safe_return_value));
    }

    {
        lock::ThreadGILRelease unlock_thread;
        return return_value_future.get();
    }
}

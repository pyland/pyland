#ifndef GIL_SAFE_FUTURE_H
#define GIL_SAFE_FUTURE_H

#include <functional>
#include <future>
#include <memory>
#include <type_traits>

#include "lifeline.hpp"

template <typename T>
class GilSafeFuture {
    private:
        GilSafeFuture(std::shared_ptr<std::promise<T>> promise);

        template <typename E=T>
        GilSafeFuture(std::shared_ptr<std::promise<T>> promise,
                      typename std::enable_if<!std::is_void<E>::value, E>::type default_value);

        std::shared_ptr<std::promise<T>> return_value_promise;
        Lifeline return_value_lifeline;

    public:
        GilSafeFuture();
        GilSafeFuture(const GilSafeFuture<T> &) = default;

        template <typename E=T, typename=typename std::enable_if<std::is_void<E>::value, E>::type>
        void set();

        template <typename E=T>
        void set(typename std::enable_if<!std::is_void<E>::value, E>::type value);

        static T execute(std::function<void (GilSafeFuture<T>)> executable);

        template <typename E=T>
        static T execute(std::function<void (GilSafeFuture<T>)> executable,
                         typename std::enable_if<!std::is_void<E>::value, E>::type default_value);
};

#include "gil_safe_future.hxx"

#endif

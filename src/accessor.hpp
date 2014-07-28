#ifndef ACCESSOR_H
#define ACCESSOR_H

// Modified accessor from http://stackoverflow.com/a/2561473/1763356,
//
// Copyright conio.
// Released under http://creativecommons.org/licenses/by-sa/2.5/

#include <functional>

template <typename T>
class Accessor {
    public:
        static const std::function<T(const T &)> default_getter;
        static const std::function<T(const T &)> default_setter;

        explicit Accessor(const T &data,
                          const std::function<T(const T &)> getter = Accessor<T>::default_getter,
                          const std::function<T(const T &)> setter = Accessor<T>::default_setter):
            value(data), getter(getter), setter(setter) {}

        Accessor &operator=(const T &data) {
            value = setter(data);
            return *this;
        }

        Accessor &operator=(const Accessor &other) {
            value = setter(other.value);
            return *this;
        }

        operator T() { return getter(value); }

    private:
        Accessor(const Accessor&);

        T value;
        const std::function<T(const T &)> getter;
        const std::function<T(const T &)> setter;
};

template <typename T>
const std::function<T(const T &)> Accessor<T>::default_getter = [] (T value) { return value; };

template <typename T>
const std::function<T(const T &)> Accessor<T>::default_setter = [] (T value) { return value; };

#endif
#ifndef PROPERTY_H
#define PROPERTY_H

#include <functional>

template <typename T, typename F=void>
class Property {
    public:
        static const std::function<T(const T &)> default_getter;
        static const std::function<T(const T &)> default_setter;

        Property(T value,
                 const std::function<T(const T &)> getter = Property<T>::default_getter,
                 const std::function<T(const T &)> setter = Property<T>::default_setter):
            value(value),
            getter(getter),
            setter(setter)
        {};

        T get() { return getter(value); }
        void set(T value) { this->value = setter(value); }

    private:
        friend class F;

        Property(const Property &) = delete;

        T value;
        const std::function<T(const T &)> getter;
        const std::function<T(const T &)> setter;
};

template <typename T>
const std::function<T(const T &)> Property<T>::default_getter = [] (T value) { return value; };

template <typename T>
const std::function<T(const T &)> Property<T>::default_setter = [] (T value) { return value; };

#endif

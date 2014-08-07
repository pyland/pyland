#include "gil_safe_future.hpp"

// TODO: Comment such that this mess makes sense.

template <>
GilSafeFuture<void>::GilSafeFuture(std::shared_ptr<std::promise<void>> promise):
    return_value_promise(promise),
    return_value_lifeline([promise] () { promise->set_value(); })
    {}

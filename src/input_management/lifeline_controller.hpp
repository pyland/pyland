#ifndef LIFELINE_CONTROLLER_H
#define LIFELINE_CONTROLLER_H

#include <memory>

#include "lifeline.hpp"



///
/// Can be used to remotely disable any lifelines which are created
/// with it.
///
class LifelineController {
private:
    friend class Lifeline::FunctionRunner;
    ///
    /// If true, the lifeline should run its function.
    ///
    std::shared_ptr<bool> allowed;
public:
    LifelineController();
    ///
    /// Stops the function being called on total destruction.
    ///
    /// Works in a similar way to Lifeline::disable(), but does not
    /// require storage of a lifeline to use, thus it can be held
    /// without inhibiting the lifeline.
    ///
    void disable();
};

#endif

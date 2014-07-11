#include <memory>

#include "lifeline_controller.hpp"



LifelineController::LifelineController():
    allowed(std::make_shared<bool>(true)) {
}

void LifelineController::disable() {
    *allowed = false;
}

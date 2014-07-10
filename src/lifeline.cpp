#include <functional>
#include <memory>

#include "lifeline.hpp"
#include "lifeline_controller.hpp"



Lifeline::Lifeline(std::function<void()> func):
    runner(std::make_shared<FunctionRunner>(func)) {
}


Lifeline::Lifeline(std::function<void()> func, LifelineController controller):
    runner(std::make_shared<FunctionRunner>(func)) {
        runner->attach_controller(controller);
}


Lifeline::FunctionRunner::FunctionRunner(std::function<void()> func):
    enabled(true),
    func(func) {
}


Lifeline::FunctionRunner::~FunctionRunner() {
    // OK, This line may need some explaining:
    // enabled is a boolean value which states whether the individual
    // lifeline / function runner should be run.
    // allowed is a shared_ptr which if invalid or references a true
    // value should allow the function to run.
    if (enabled && (!allowed || *allowed)) {
        func();
    }
}


void Lifeline::disable() {
    runner->enabled = false;
}


void Lifeline::FunctionRunner::attach_controller (LifelineController controller) {
    allowed = controller.allowed;
}

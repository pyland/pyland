#include <functional>
#include <memory>

#include "lifeline.hpp"



Lifeline::Lifeline(std::function<void()> func):
    runner(std::make_shared<FunctionRunner>(func)) {
}


Lifeline::FunctionRunner::FunctionRunner(std::function<void()> func):
    func(func),
    enabled(true) {
}


Lifeline::FunctionRunner::~FunctionRunner() {
    if (enabled) {
        func();
    }
}


void Lifeline::disable() {
    runner->controller->enabled = false;
}


void Lifeline::Controller::disable() {
    enabled = false;
}


std::shared_pointer<Controller> Lifeline::get_controller () {
    return runner->controller;
}

#include "lifetime.hpp"



Lifeline::Lifeline(std::function<void()> func) : runner(make_shared<FunctionRunner>(func)) {
}


Lifeline::FunctionRunner::FunctionRunner(std::function<void()> func) : func(func) {
}


Lifeline::FunctionRunner::~FunctionRunner() {
    func();
}



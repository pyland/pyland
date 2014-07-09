#ifndef LIFELINE_H
#define LIFELINE_H

#include <functional>
#include <memory>



///
/// On destruction of all instances, the object runs a given function.
///
class Lifeline {
private:
    class Controller;
    
    ///
    /// There is one instance of this class for all clones of the
    /// container class. Upon destruction, the function is run.
    ///
    class FunctionRunner {
    private:
        friend class Lifeline;
        ///
        /// Controls whether the function should be run on destruction.
        ///
        std::shared_ptr<Controller> controller;
        ///
        /// Function to run upon full destruction.
        ///
        std::function<void()> func;
    public:
        FunctionRunner(std::function<void()> func);
        ~FunctionRunner();
    };
    ///
    /// Shared pointers will run the destructor when the reference count
    /// reaches zero.
    ///
    std::shared_ptr<FunctionRunner> runner;
public:
    class Controller {
    private:
        ///
        /// If true, the lifeline should run its function.
        ///
        bool enabled;
    public:
        ///
        /// Stops the function being called on total destruction.
        ///
        /// Works in a similar way to Lifeline::disable(), but does not
        /// require storage of a lifeline to use, thus it can be held
        /// without inhibiting the lifeline.
        ///
        void disable();
    }
        
    Lifeline(std::function<void()> func);
    
    ///
    /// Stops the function being called on total destruction.
    ///
    /// This is useful if running the function could cause instability,
    /// for example, if the function was going to access something which
    /// was no longer accessible.
    ///
    void disable();

    ///
    /// Returns a shared pointer to a controller, which can disable the
    /// lifeline.
    ///
    std::shared_pointer<Controller> get_controller ();
};
    
#endif

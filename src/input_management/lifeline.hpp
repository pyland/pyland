#ifndef LIFELINE_H
#define LIFELINE_H

#include <functional>
#include <memory>

class LifelineController;

///
/// On destruction of all instances, the object runs a given function.
///
class Lifeline {
private:
    friend class LifelineController;
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
        /// If valid and false, the function will not run. This is
        /// controlled by a controller object.
        ///
        std::shared_ptr<bool> allowed;
        ///
        /// Controls whether the function should be run on destruction.
        ///
        /// If false, the function will not run. This is for
        /// individualistic disabling.
        ///
        bool enabled;
        ///
        /// Function to run upon full destruction.
        ///
        std::function<void()> func;
    public:
        FunctionRunner(std::function<void()> func);
        ~FunctionRunner();

        ///
        /// Specifies a controller, which may be used to disable the
        /// lifeline.
        ///
        void attach_controller (LifelineController controller);
    };
    ///
    /// Shared pointers will run the destructor when the reference count
    /// reaches zero.
    ///
    std::shared_ptr<FunctionRunner> runner;
public:
    Lifeline(std::function<void()> func);
    ///
    /// Construct a lifeline which can be disabled using a controller.
    ///
    Lifeline(std::function<void()> func, LifelineController controller);
    
    ///
    /// Stops the function being called on total destruction.
    ///
    /// This is useful if running the function could cause instability,
    /// for example, if the function was going to access something which
    /// was no longer accessible.
    ///
    void disable();
};
    
#endif

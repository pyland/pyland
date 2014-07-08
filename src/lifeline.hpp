#ifndef CALLBACK_H
#define CALLBACK_H



///
/// On destruction of all instances, the object runs a given function.
///
class Lifeline {
private:
    ///
    /// There is one instance of this class for all clones of the
    /// container class. Upon destruction, the function is run.
    class FunctionRunner {
    private:
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
    Lifeline(std::function<void()> func);
}
    
#endif

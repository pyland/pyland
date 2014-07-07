template <class T>
struct Lockable {
    T items;
    std::mutex lock;
};
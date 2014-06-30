#include <thread>

class PlayerThread {
    public:
        PlayerThread(Player &player, std::unique_ptr<std::thread> thread, int64_t thread_id);

        Player &player;
        std::unique_ptr<std::thread> thread;

        // Largest definitely-available integer,
        // gives contingency because a large number
        // of calls to APIs could occur.
        uint64_t previous_call_number;

        // Largest definitely-available integer,
        // so can probably hold the given Python int.
        //
        // Not using unsigned integers as I don't know
        // if thread ids can be negative or not.
        int64_t thread_id;

        bool is_dirty();
        void set_clean();
};

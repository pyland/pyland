#include <thread>

class PlayerThread {
    public:
        ///
        /// Construct a PlayerThread from a Player object.
        ///
        /// Spawns a new thread.
        ///
        PlayerThread(Player &player);

        ///
        /// Try to nicely kill the thread; avoiding corruption where possible.
        ///
        /// Neither termination nor safety is guaranteed.
        ///
        void halt_soft();

        ///
        /// Try to kill the thread without mortal concerns for such things as life and death.
        ///
        /// Neither termination nor safety is guaranteed, although
        /// termination is more likely than with halt_soft.
        ///
        void halt_hard();

        ///
        /// Check if an API call has been made since last call to clean.
        ///
        /// @return Whether API call has been made since last call to clean.
        ///
        bool is_dirty();

        ///
        /// Remember state when last called, for use with is_dirty.
        ///
        /// @see is_dirty
        ///
        void clean();

        void finish();

    private:
        /// 
        /// Reference to API's wrapped Entity.
        ///
        /// API calls are passed through to this object.
        /// 
        Player &player;

        ///
        /// Thread spawned by this PlayerThread
        ///
        std::unique_ptr<std::thread> thread;

        ///
        /// Largest definitely-available integer,
        /// gives contingency because a large number
        /// of calls to APIs could occur.
        ///
        uint64_t previous_call_number;

        ///
        /// Largest definitely-available integer,
        /// so can probably hold the given Python int.
        ///
        /// Not using unsigned integers as I don't know
        /// if thread ids can be negative or not.
        ///
        int64_t thread_id;
};

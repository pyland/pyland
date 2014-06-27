class PlayerThread {
	private:
		uint64_t previous_call_number;

	public:
		Player player;
		std::thread thread;

		// Largest definitely-available integer,
		// so can probably hold the given Python int.
		//
		// Not using unsigned integers as I don't know
		// if thread ids can be negative or not.
		int64_t thread_id;

		bool is_dirty();
		void set_clean();
}
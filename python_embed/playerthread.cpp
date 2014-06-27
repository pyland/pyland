PlayerThread(Player &player, std::thread &thread, int64_t thread_id) {
	this.player = player;
	this.thread = thread;
	this.thread_id = thread_id;
};

bool PlayerThread::is_dirty() {
	return previous_call_number != player.call_number;
}

void PlayerThread::set_clean() {
	previous_call_number = player.call_number;
}

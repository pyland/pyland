bool PlayerThread::is_dirty() {
	return previous_call_number == player.call_number;
}

void PlayerThread::set_clean() {
	previous_call_number = player.call_number;
}
